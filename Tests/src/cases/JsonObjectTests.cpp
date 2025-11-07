#include <gtest/gtest.h>

#include "JsonParser.h"
#include "JsonArrayWrapper.h"

TEST(Object, DefaultOperations)
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	json::JsonObject object;
	std::string data;

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
