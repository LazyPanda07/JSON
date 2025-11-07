#include <fstream>

#include <gtest/gtest.h>

#include "Utils.h"

TEST(Parser, Getters)
{
	json::JsonParser parser(createParser());

	ASSERT_EQ(parser.get<std::nullptr_t>("nullValue"), nullptr);
	ASSERT_EQ(parser.get<bool>("boolValue"), true);
	ASSERT_EQ(parser.get<int>("intValue"), 5);
	ASSERT_EQ(parser.get<double>("doubleValue"), 10.2);
	ASSERT_EQ(parser.get<uint32_t>("unsignedIntValue"), 15);
	ASSERT_EQ(parser.get<std::string>("stringValue"), "qwe");

	EXPECT_THROW(parser.get<int>("test"), json::exceptions::CantFindValueException);
	EXPECT_THROW(parser.get<nullptr_t>("intValue"), std::bad_variant_access);
}

TEST(Parser, TryGetters)
{
	json::JsonParser parser(createParser());
	bool boolValue;
	int16_t intValue;
	float floatValue;
	uint16_t unsignedIntValue;
	std::string stringValue;
	nullptr_t null;

	ASSERT_TRUE(parser.tryGet("nullValue", null));
	ASSERT_TRUE(parser.tryGet("boolValue", boolValue));
	ASSERT_TRUE(parser.tryGet("intValue", intValue));
	ASSERT_TRUE(parser.tryGet("doubleValue", floatValue));
	ASSERT_TRUE(parser.tryGet("unsignedIntValue", unsignedIntValue));
	ASSERT_TRUE(parser.tryGet("stringValue", stringValue));

	ASSERT_FALSE(parser.tryGet("test", intValue));
	ASSERT_FALSE(parser.tryGet("doubleValue", stringValue));
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

	json::JsonParser parser(jsonData);

	parser.overrideValue("intValue", 10LL);
	parser.overrideValue("stringValue", true);

	parser.overrideValue("doubleValue", 13.2, true);

	ASSERT_EQ(parser.get<int64_t>("intValue"), 10);
	ASSERT_EQ(parser.get<bool>("stringValue"), true);
	ASSERT_EQ(parser.get<double>("doubleValue", true), 13.2);

	ASSERT_THROW(parser.overrideValue("doubleValue", 13.2), json::exceptions::CantFindValueException);
}

TEST(Parser, StreamOperators)
{
	json::JsonParser parser;
	std::stringstream first;
	std::stringstream second;

	{
		std::ifstream in("data/appends.json");

		in >> parser;
	}

	std::ifstream in("data/appends.json");

	first << parser;

	parser = json::JsonParser(first);

	ASSERT_EQ(parser.get<std::nullptr_t>("nullValue"), nullptr);
	ASSERT_EQ(parser.get<bool>("boolValue"), true);
	ASSERT_EQ(parser.get<int>("intValue"), 5);
	ASSERT_EQ(parser.get<double>("doubleValue"), 10.2);
	ASSERT_EQ(parser.get<uint32_t>("unsignedIntValue"), 15);
	ASSERT_EQ(parser.get<std::string>("stringValue"), "qwe");

	{
		const json::JsonObject& object = parser.get<json::JsonObject>("objectValue");

		ASSERT_EQ(object["nullValue"].get<std::nullptr_t>(), nullptr);
		ASSERT_EQ(object["boolValue"].get<bool>(), true);
		ASSERT_EQ(object["intValue"].get<int>(), 5);
		ASSERT_EQ(object["doubleValue"].get<double>(), 10.2);
		ASSERT_EQ(object["unsignedIntValue"].get<uint32_t>(), 15);
		ASSERT_EQ(object["stringValue"].get<std::string>(), "qwe");
	}

	{
		const std::vector<json::JsonObject>& array = parser.get<std::vector<json::JsonObject>>("arrayValue");

		ASSERT_EQ(array[0].get<std::nullptr_t>(), nullptr);
		ASSERT_EQ(array[1].get<bool>(), true);
		ASSERT_EQ(array[2].get<int>(), 5);
		ASSERT_EQ(array[3].get<double>(), 10.2);
		ASSERT_EQ(array[4].get<uint32_t>(), 15);
		ASSERT_EQ(array[5].get<std::string>(), "qwe");

		const json::JsonObject& object = array[6].get<json::JsonObject>();

		ASSERT_EQ(object["nullValue"].get<std::nullptr_t>(), nullptr);
		ASSERT_EQ(object["boolValue"].get<bool>(), true);
		ASSERT_EQ(object["intValue"].get<int>(), 5);
		ASSERT_EQ(object["doubleValue"].get<double>(), 10.2);
		ASSERT_EQ(object["unsignedIntValue"].get<uint32_t>(), 15);
		ASSERT_EQ(object["stringValue"].get<std::string>(), "qwe");
	}
}

TEST(Parser, Contains)
{
	json::JsonParser parser(std::ifstream("data/contains.json"));

	ASSERT_TRUE(parser.contains<int>("someData"));
	ASSERT_TRUE(parser.contains<float>("someRecursiveData", true));
	ASSERT_TRUE(parser.contains<std::string>("someRecursiveDataInArray", true));

	ASSERT_FALSE(parser.contains<int>("data"));
	ASSERT_FALSE(parser.contains<float>("someRecursiveData"));
	ASSERT_FALSE(parser.contains<std::string>("someRecursiveDataInArray"));
}

TEST(Parser, SimpleLineComment) 
{
    std::string input = R"({
        // single line
        "a": 1
    })";
    
	json::JsonParser parser(input);

    EXPECT_EQ(parser.get<int>("a"), 1);
}

TEST(Parser, InlineComment) 
{
    std::string input = R"({
        "a": 1, // after field
        "b": 2
    })";
    std::string expected = R"({
        "a": 1, 
        "b": 2
    })";

	json::JsonParser parser(input);

    EXPECT_EQ(parser.get<int>("a"), 1);
    EXPECT_EQ(parser.get<int>("b"), 2);
}

TEST(Parser, MultiLineComment) 
{
    std::string input = R"({
        /* multi
           line
           comment */
        "a": 1
    })";
    std::string expected = R"({
        
        "a": 1
    })";

	json::JsonParser parser(input);

	EXPECT_EQ(parser.get<int>("a"), 1);
}

TEST(Parser, MixedComments) 
{
    std::string input = R"({
        // start comment
        "a": 1, /* inline */
        "b": 2 // end
    })";
    std::string expected = R"({
        
        "a": 1, 
        "b": 2 
    })";

	json::JsonParser parser(input);

	EXPECT_EQ(parser.get<int>("a"), 1);
	EXPECT_EQ(parser.get<int>("b"), 2);
}

TEST(Parser, TrickyStars) 
{
    std::string input = R"({
        /* comment ** with stars **/
        "a": 1
    })";
    std::string expected = R"({
        
        "a": 1
    })";

	json::JsonParser parser(input);

	EXPECT_EQ(parser.get<int>("a"), 1);
}
