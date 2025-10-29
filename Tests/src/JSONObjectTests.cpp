#include <gtest/gtest.h>

#include "JsonParser.h"
#include "JsonArrayWrapper.h"

TEST(Object, DefaultOperations)
{
	json::JsonObject object;
	std::string data;
	std::nullptr_t null;

	object.setValue<int64_t>("int", 5);
	object.setValue<std::string>("string", "data");
	object.setValue<std::string_view>("string_view", "view");

	{
		json::JsonObject temp;
		std::vector<json::JsonObject> array;

		json::JsonObject::appendArray(5, array);
		json::JsonObject::appendArray("data", array);
		json::JsonObject::appendArray(nullptr, array);

		temp.setValue("float", 5.5);
		temp.setValue("array", std::move(array));

		object.setValue("object", std::move(temp));
	}

	ASSERT_EQ(object.get<int>("int"), 5);
	ASSERT_TRUE(object.tryGet<std::string>("string", data));
	ASSERT_EQ(data, "data");
	ASSERT_EQ(object.get<std::string>("string_view"), "view");
	ASSERT_FALSE(object.tryGet<std::nullptr_t>("null", null));

	{
		const json::JsonObject& temp = object.get<json::JsonObject>("object");
		const std::vector<json::JsonObject>& array = temp.get<std::vector<json::JsonObject>>("array");
		json::utility::JsonArrayWrapper wrapper(array);

		ASSERT_EQ(temp.get<float>("float"), 5.5);

		ASSERT_EQ(std::get<int64_t>(wrapper[0]), 5);
		ASSERT_EQ(std::get<std::string>(wrapper[1]), "data");
		ASSERT_EQ(std::get<nullptr_t>(wrapper[2]), nullptr);
	}
}
