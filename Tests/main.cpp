#include "gtest/gtest.h"

#include "Utils.h"

#include "JsonParser.h"

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	json::JsonParser parser = createParser();
	const json::JsonObject& object = parser.get<json::JsonObject>("objectValue");

	return RUN_ALL_TESTS();
}
