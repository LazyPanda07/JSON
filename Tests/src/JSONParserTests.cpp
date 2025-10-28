#include <fstream>

#include <gtest/gtest.h>

#include "Utils.h"

TEST(Parser, Getters)
{
	json::JSONParser parser(createParser());

	ASSERT_EQ(parser.get<std::nullptr_t>("nullValue"), nullptr);
	ASSERT_EQ(parser.get<bool>("boolValue"), true);
	ASSERT_EQ(parser.get<int>("intValue"), 5);
	ASSERT_EQ(parser.get<double>("doubleValue"), 10.2);
	ASSERT_EQ(parser.get<uint32_t>("unsignedIntValue"), 15);
	ASSERT_EQ(parser.get<std::string>("stringValue"), "qwe");

	EXPECT_THROW(parser.getInt("test"), json::exceptions::CantFindValueException);
	EXPECT_THROW(parser.getNull("intValue"), std::bad_variant_access);
}

TEST(Parser, TryGetters)
{
	json::JSONParser parser(createParser());
	bool boolValue;
	int16_t intValue;
	float floatValue;
	uint16_t unsignedIntValue;
	std::string stringValue;

	ASSERT_TRUE(parser.tryGetNull("nullValue"));
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

	json::JSONParser parser(jsonData);

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

TEST(Parser, SimpleLineComment) 
{
    std::string input = R"({
        // single line
        "a": 1
    })";
    
	json::JSONParser parser(input);

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

	json::JSONParser parser(input);

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

	json::JSONParser parser(input);

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

	json::JSONParser parser(input);

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

	json::JSONParser parser(input);

	EXPECT_EQ(parser.get<int>("a"), 1);
}
