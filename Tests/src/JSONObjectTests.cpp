#include <gtest/gtest.h>

#include "JSONParser.h"

TEST(Object, DefaultOperations)
{
	json::JsonObject object;
	std::string data;
	std::nullptr_t null;

	object.setInt("int", 5);
	object.setString("string", "data");

	ASSERT_EQ(object.get<int>("int"), 5);
	ASSERT_TRUE(object.tryGet<std::string>("string", data));
	ASSERT_EQ(data, "data");
	ASSERT_FALSE(object.tryGet<std::nullptr_t>("null", null));
}
