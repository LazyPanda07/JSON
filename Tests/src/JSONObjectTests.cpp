#include <gtest/gtest.h>

#include "JsonParser.h"

TEST(Object, DefaultOperations)
{
	json::JsonObject object;
	std::string data;
	std::nullptr_t null;

	object.setValue<int64_t>("int", 5);
	object.setValue<std::string>("string", "data");
	object.setValue<std::string_view>("string_view", "view");

	ASSERT_EQ(object.get<int>("int"), 5);
	ASSERT_TRUE(object.tryGet<std::string>("string", data));
	ASSERT_EQ(data, "data");
	ASSERT_EQ(object.get<std::string>("string_view"), "view");
	ASSERT_FALSE(object.tryGet<std::nullptr_t>("null", null));
}
