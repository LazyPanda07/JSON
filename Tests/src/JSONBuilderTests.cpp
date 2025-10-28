#include <fstream>

#include <gtest/gtest.h>

#include "JsonBuilder.h"
#include "Exceptions/CantFindValueException.h"

using namespace std::string_literals;

TEST(Builder, Appends)
{
	std::vector<json::JsonObject> array;
	json::JsonObject object;
	std::ostringstream jsonData;
	std::ifstream referenceData("data/appends.json");

	ASSERT_TRUE(referenceData.is_open());

	object.setNull("nullValue");
	object.setBool("boolValue", true);
	object.setInt("intValue", 5);
	object.setDouble("doubleValue", 10.2);
	object.setUnsignedInt("unsignedIntValue", 15);
	object.setString("stringValue", "qwe");

	json::JsonObject::appendArray(nullptr, array);
	json::JsonObject::appendArray(true, array);
	json::JsonObject::appendArray(5LL, array);
	json::JsonObject::appendArray(10.2, array);
	json::JsonObject::appendArray(15ULL, array);
	json::JsonObject::appendArray("qwe"s, array);
	json::JsonObject::appendArray(object, array);

	jsonData << json::JsonBuilder(CP_UTF8)
		.appendNull("nullValue")
		.appendBool("boolValue", true)
		.appendInt("intValue", 5)
		.appendDouble("doubleValue", 10.2)
		.appendUnsignedInt("unsignedIntValue", 15)
		.appendString("stringValue", "qwe")
		.appendArray("arrayValue", std::move(array))
		.appendObject("objectValue", std::move(object));

	ASSERT_EQ
	(
		(std::ostringstream() << referenceData.rdbuf()).str(),
		jsonData.str()
	);
}

TEST(Builder, Contains)
{
	json::JsonBuilder builder(CP_UTF8);
	json::JsonObject object;

	object.setInt("someRecursiveData", 10);

	builder["someData"] = 5LL;
	builder["object"] = std::move(object);

	ASSERT_TRUE(builder.contains("someData", json::utility::VariantTypeEnum::jInt64_t));
	ASSERT_TRUE(builder.contains("someRecursiveData", json::utility::VariantTypeEnum::jInt64_t, true));
	ASSERT_FALSE(builder.contains("someRecursiveData", json::utility::VariantTypeEnum::jInt64_t));
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
	builder["stringValue"] = "qwe"s;

	ASSERT_EQ(std::get<std::nullptr_t>(builder["nullValue"]), nullptr);
	ASSERT_EQ(std::get<bool>(builder["boolValue"]), true);
	ASSERT_EQ(std::get<int64_t>(builder["intValue"]), 5LL);
	ASSERT_EQ(std::get<double>(builder["doubleValue"]), 10.2);
	ASSERT_EQ(std::get<uint64_t>(builder["unsignedIntValue"]), 15ULL);
	ASSERT_EQ(std::get<std::string>(builder["stringValue"]), "qwe");

	ASSERT_EQ(std::get<std::nullptr_t>(builder["qwe"]), nullptr);

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
	std::ostringstream referenceData;

	referenceData << std::ifstream("data/minimize.json").rdbuf();

	object.setInt("someRecursiveData", 10);

	builder["someData"] = 5LL;
	builder["object"] = std::move(object);

	builder.minimize();

	ASSERT_EQ(referenceData.str(), builder.build());
}
