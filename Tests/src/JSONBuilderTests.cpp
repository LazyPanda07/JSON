#include <fstream>

#include <gtest/gtest.h>

#include "JsonBuilder.h"
#include "JsonParser.h"
#include "Exceptions/CantFindValueException.h"

TEST(Builder, Appends)
{
	std::vector<json::JsonObject> array;
	json::JsonObject object;
	std::stringstream jsonData;
	json::JsonParser reference(std::ifstream("data/appends.json"));

	object["nullValue"] = nullptr;
	object["boolValue"] = true;
	object["intValue"] = 5;
	object["doubleValue"] = 10.2;
	object["unsignedIntValue"] = 15;
	object["stringValue"] = "qwe";

	array.emplace_back(nullptr);
	array.emplace_back(true);
	array.emplace_back(5LL);
	array.emplace_back(10.2);
	array.emplace_back(15ULL);
	array.emplace_back("qwe");
	array.emplace_back(object);

	jsonData << json::JsonBuilder(CP_UTF8)
		.append<std::nullptr_t>("nullValue")
		.append("boolValue", true)
		.append("intValue", 5)
		.append("doubleValue", 10.2)
		.append("unsignedIntValue", 15)
		.append("stringValue", "qwe")
		.append("arrayValue", std::move(array))
		.append("objectValue", std::move(object));

	ASSERT_EQ(reference.getParsedData(), json::JsonParser(jsonData).getParsedData());
}

TEST(Builder, Contains)
{
	json::JsonBuilder builder(CP_UTF8);
	json::JsonObject object;

	object["someRecursiveData"] = 10;

	builder["someData"] = 5LL;
	builder["object"] = std::move(object);

	ASSERT_TRUE(builder.contains<int>("someData"));
	ASSERT_TRUE(builder.contains<int>("someRecursiveData", true));
	ASSERT_FALSE(builder.contains<int>("someRecursiveData"));
}

TEST(Builder, Operators)
{
	json::JsonBuilder builder(CP_UTF8);
	const json::JsonBuilder& constReference = builder;

	builder["nullValue"] = nullptr;
	builder["boolValue"] = true;
	builder["intValue"] = 5LL;
	builder["doubleValue"] = 10.2;
	builder["unsignedIntValue"] = 15ULL;
	builder["stringValue"] = "qwe";

	ASSERT_EQ(builder["nullValue"].get<std::nullptr_t>(), nullptr);
	ASSERT_EQ(builder["boolValue"].get<bool>(), true);
	ASSERT_EQ(builder["intValue"].get<int>(), 5);
	ASSERT_EQ(builder["doubleValue"].get<double>(), 10.2);
	ASSERT_EQ(builder["unsignedIntValue"].get<uint64_t>(), 15);
	ASSERT_EQ(builder["stringValue"].get<std::string>(), "qwe");

	ASSERT_EQ(builder["qwe"].get<std::nullptr_t>(), nullptr);

	EXPECT_THROW
	(
		constReference["123"],
		json::exceptions::CantFindValueException
	);
}

TEST(Builder, Minimize)
{
	json::JsonBuilder builder(CP_UTF8);
	json::JsonObject object;
	json::JsonParser reference(std::ifstream("data/minimize.json"));

	object["someRecursiveData"] = 10;

	builder["someData"] = 5;
	builder["object"] = std::move(object);

	builder.minimize();

	ASSERT_EQ(reference.getParsedData(), json::JsonParser(builder.build()).getParsedData());
}
