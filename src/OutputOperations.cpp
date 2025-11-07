#include "OutputOperations.h"

#include <format>

namespace json::utility
{
	std::ostream& outputJsonType(std::ostream& outputStream, const JsonObject& value, bool isLast, std::string& offset)
	{
		if (value.is<JsonVariantTypeEnum::jJSONArray>() || value.is<JsonVariantTypeEnum::jJSONObject>())
		{
			offset += "  ";
		}

		JsonVariantTypeEnum type = value.getEnumType();

		switch (type)
		{
		case JsonVariantTypeEnum::jNull:
			outputStream << "null";

			break;

		case JsonVariantTypeEnum::jString:
			outputStream << '"' << std::regex_replace(value.get<std::string>(), std::regex(R"(\\)"), R"(\\)") << '"';

			break;

		case JsonVariantTypeEnum::jBool:
			outputStream << std::boolalpha << value.get<bool>();

			break;

		case JsonVariantTypeEnum::jInt:
			outputStream << value.get<int64_t>();

			break;

		case JsonVariantTypeEnum::jUint:
			outputStream << value.get<uint64_t>();

			break;

		case JsonVariantTypeEnum::jDouble:
			outputStream << std::fixed << value.get<double>();

			break;

		case JsonVariantTypeEnum::jJSONArray:
			outputStream << JsonArrayWrapper(value, &offset) << std::string_view(offset.begin(), offset.end() - 2) << ']';

			break;

		case JsonVariantTypeEnum::jJSONObject:
		{
			JsonObject::ConstIterator begin = value.begin();
			JsonObject::ConstIterator end = value.end();

			outputStream << "{\n";

			while (begin != end)
			{
				JsonObject::ConstIterator check = begin;
				const JsonObject& temp = *check;

				if (std::optional<std::string_view> key = check.key())
				{
					outputStream << std::format(R"({}"{}": )", offset, *key);
				}
				else
				{
					outputStream << offset;
				}

				outputJsonType(outputStream, temp, ++check == end, offset);

				++begin;
			}

			outputStream << std::string(offset.begin(), offset.end() - 2) << '}';
		}

		break;
		}

		if (value.is<JsonVariantTypeEnum::jJSONArray>() || value.is<JsonVariantTypeEnum::jJSONObject>())
		{
			offset.pop_back();
			offset.pop_back();
		}

		if (!isLast)
		{
			outputStream << ',';
		}

		return outputStream << std::endl;
	}
}
