#include <gtest/gtest.h>

#include <fstream>

#include "JsonParser.h"
#include "JsonArrayWrapper.h"
#include "RecursiveJsonIterator.h"
#include "MapJsonIterator.h"

TEST(Object, DefaultOperations)
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	json::JsonObject object;
	std::string data;
	int intData;

	object["int"] = 5;
	object["string"] = "data"s;
	object["string_view"] = "view"sv;
	object["ptr"] = "ptr";

	{
		json::JsonObject temp;
		std::vector<json::JsonObject> array;

		array.emplace_back(5);
		array.emplace_back("data");
		array.emplace_back(nullptr);

		temp["float"] = 5.5;
		temp["array"] = std::move(array);

		object["object"] = std::move(temp);
	}

	ASSERT_EQ(object["int"].get<int>(), 5);
	ASSERT_TRUE(object["string"].tryGet<std::string>(data));
	ASSERT_TRUE(object.tryGet<std::string>("string", data));
	ASSERT_FALSE(object["string"].tryGet<int>(intData));
	ASSERT_FALSE(object.tryGet<int>("string", intData));
	ASSERT_EQ(data, "data");
	ASSERT_EQ(object["string_view"].get<std::string>(), "view");
	ASSERT_EQ(object["ptr"].get<std::string>(), "ptr");
	ASSERT_FALSE(object.contains<std::nullptr_t>("null"));
	ASSERT_FALSE(object.contains<std::nullptr_t>("null", true));

	{
		const json::JsonObject& temp = object["object"].get<json::JsonObject>();
		const std::vector<json::JsonObject>& array = temp["array"].get<std::vector<json::JsonObject>>();
		json::utility::JsonArrayWrapper wrapper(array);

		ASSERT_EQ(temp["float"].get<float>(), 5.5);

		ASSERT_EQ(wrapper[0].get<int64_t>(), 5);
		ASSERT_EQ(wrapper[1].get<std::string>(), "data");
		ASSERT_EQ(wrapper[2].get<std::nullptr_t>(), nullptr);
	}
}

TEST(Object, RecursiveIterator)
{
	json::JsonParser parser(std::ifstream("data/appends.json"));
	json::RecursiveJsonIterator recursiveIterator(parser);
	std::vector<std::string> referenceData =
	{
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(doubleValue: 10.200000)",
		R"(stringValue: "qwe")",
		R"(unsignedIntValue: 15)",
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(doubleValue: 10.200000)",
		R"(stringValue: "qwe")",
		R"(unsignedIntValue: 15)",
		R"(null)",
		R"(true)",
		R"(5)",
		R"(10.200000)",
		R"(15)",
		R"("qwe")",
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(stringValue: "qwe")",
		R"(doubleValue: 10.200000)",
		R"(unsignedIntValue: 15)"
	};

	for (const auto& it : recursiveIterator)
	{
		std::ostringstream os;

		if (std::optional<std::string_view> key = it.key())
		{
			os << std::format("{}: ", *key);
		}

		os << *it;

		std::string value(os.str());

		if (value.ends_with('\n'))
		{
			value.pop_back();
		}

		std::erase(referenceData, value);
	}

	ASSERT_EQ(referenceData.size(), 0);
}

TEST(Object, MapIterator)
{
	json::JsonObject data;
	auto createMapJsonIterator = [](const json::JsonObject& data) -> json::MapJsonIterator
		{
			return json::MapJsonIterator(data);
		};
	std::unordered_map<std::string, std::any> reference =
	{
		{ "first", std::string("first") },
		{ "bool", true },
		{ "real", 3.5 },
		{ "integer", static_cast<int64_t>(15) },
		{ "array", json::JsonObject::makeArray("first", true, 3.5f, 15) },
	};

	ASSERT_THROW(createMapJsonIterator(data), std::runtime_error);

	data["first"] = "first";
	data["bool"] = true;
	data["real"] = 3.5f;
	data["integer"] = 15;
	data["array"] = json::JsonObject::makeArray("first", true, 3.5f, 15);

	json::MapJsonIterator mapIterator = createMapJsonIterator(data);

	for (const auto& [key, value] : mapIterator)
	{
		const std::any& referenceValue = reference.at(key);

		ASSERT_EQ(referenceValue.type().name(), value.getType().name()) << key;
	}

	for (const auto& [key, value] : mapIterator)
	{
		const std::any& referenceValue = reference.at(key);

		ASSERT_EQ(referenceValue.type().name(), value.getType().name()) << key;
	}
}

TEST(Object, EmplaceBack)
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	json::JsonObject array;
	
	array.emplace_back(1);
	array.emplace_back(5.5);
	array.emplace_back("first");
	array.emplace_back("second"s);
	array.emplace_back("third"sv);
	array.emplace_back(nullptr);

	ASSERT_TRUE(array[0].is<int>());
	ASSERT_TRUE(array[1].is<float>());
	ASSERT_TRUE(array[2].is<std::string>());
	ASSERT_TRUE(array[3].is<std::string>());
	ASSERT_TRUE(array[4].is<std::string>());
	ASSERT_TRUE(array[5].is<std::nullptr_t>());
}

TEST(Object, Size)
{
	json::JsonObject array;
	json::JsonObject map;

	array.emplace_back(1);
	array.emplace_back(5.5);

	map["first"] = 1;
	map["second"] = 5.5;

	ASSERT_EQ(array.size(), 2);
	ASSERT_EQ(map.size(), 2);
}
