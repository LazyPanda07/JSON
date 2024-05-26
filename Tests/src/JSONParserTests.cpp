#include <fstream>

#include "gtest/gtest.h"

#include "Utils.h"

TEST(Parser, Getters)
{
	json::JSONParser parser(createParser());

	ASSERT_EQ(parser.getNull("nullValue"), nullptr);
	ASSERT_EQ(parser.getBool("boolValue"), true);
	ASSERT_EQ(parser.getInt("intValue"), 5);
	ASSERT_EQ(parser.getDouble("doubleValue"), 10.2);
	ASSERT_EQ(parser.getUnsignedInt("unsignedIntValue"), 15);
	ASSERT_EQ(parser.getString("stringValue"), "qwe");

	EXPECT_THROW(parser.getInt("test"), json::exceptions::CantFindValueException);
	EXPECT_THROW(parser.getNull("intValue"), std::bad_variant_access);
}

TEST(Parser, TryGetters)
{
	json::JSONParser parser(createParser());
	bool boolValue;
	int64_t intValue;
	double doubleValue;
	uint64_t unsignedIntValue;
	std::string stringValue;

	ASSERT_TRUE(parser.tryGetNull("nullValue"));
	ASSERT_TRUE(parser.tryGetBool("boolValue", boolValue));
	ASSERT_TRUE(parser.tryGetInt("intValue", intValue));
	ASSERT_TRUE(parser.tryGetDouble("doubleValue", doubleValue));
	ASSERT_TRUE(parser.tryGetUnsignedInt("unsignedIntValue", unsignedIntValue));
	ASSERT_TRUE(parser.tryGetString("stringValue", stringValue));

	ASSERT_FALSE(parser.tryGetInt("test", intValue));
	ASSERT_FALSE(parser.tryGetString("doubleValue", stringValue));
}

TEST(Parser, Override)
{
	std::string jsonData = R"({
	"intValue": 5,
	"stringValue": "data",
	"object": {
		"doubleValue": 5.5
	}
})";

	json::JSONParser parser(jsonData);

	parser.overrideValue("intValue", 10LL);
	parser.overrideValue("stringValue", true);

	parser.overrideValue("doubleValue", 13.2, true);

	ASSERT_EQ(parser.getInt("intValue"), 10);
	ASSERT_EQ(parser.getBool("stringValue"), true);
	ASSERT_EQ(parser.getDouble("doubleValue", true), 13.2);

	ASSERT_THROW(parser.overrideValue("doubleValue", 13.2), json::exceptions::CantFindValueException);
}

TEST(Parser, StreamOperators)
{
	json::JSONParser parser;
	std::ostringstream first;
	std::ostringstream second;

	{
		std::ifstream in("data/appends.json");

		in >> parser;
	}

	std::ifstream in("data/appends.json");

	first << parser;
	second << in.rdbuf();

	ASSERT_EQ(first.str(), second.str());
}
