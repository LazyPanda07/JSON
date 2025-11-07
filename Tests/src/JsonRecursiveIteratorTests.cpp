#include <fstream>

#include <gtest/gtest.h>

#include "JsonParser.h"
#include "RecursiveJsonIterator.h"

TEST(RecursiveIterator, Traverse)
{
	json::JsonParser parser(std::ifstream("data/appends.json"));
	json::RecursiveJsonIterator recursiveIterator(parser);
	std::vector<std::string> referenceData =
	{
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(doubleValue: 10.200000)",
		R"(stringValue: "qwe")",
		R"(unsignedIntValue: 15)",
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(doubleValue: 10.200000)",
		R"(stringValue: "qwe")",
		R"(unsignedIntValue: 15)",
		R"(null)",
		R"(true)",
		R"(5)",
		R"(10.200000)",
		R"(15)",
		R"("qwe")",
		R"(nullValue: null)",
		R"(boolValue: true)",
		R"(intValue: 5)",
		R"(stringValue: "qwe")",
		R"(doubleValue: 10.200000)",
		R"(unsignedIntValue: 15)"
	};

	for (const auto& it : recursiveIterator)
	{
		std::ostringstream os;
		
		if (std::optional<std::string_view> key = it.key())
		{
			os << std::format("{}: ", *key);
		}

		os << *it;

		std::string value(os.str());

		if (value.ends_with('\n'))
		{
			value.pop_back();
		}

		std::erase(referenceData, value);
	}

	ASSERT_EQ(referenceData.size(), 0);
}
