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

			outputStream << std::string_view(offset.begin(), offset.end() - 2) << '}';
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

	std::string outputJsonType(const JsonObject& value, bool isLast, std::string& offset)
	{
		std::string result;

		if (value.is<JsonVariantTypeEnum::jJSONArray>() || value.is<JsonVariantTypeEnum::jJSONObject>())
		{
			offset += "  ";
		}

		JsonVariantTypeEnum type = value.getEnumType();

		switch (type)
		{
		case JsonVariantTypeEnum::jNull:
			result += "null";

			break;

		case JsonVariantTypeEnum::jString:
			result += std::format(R"("{}")", std::regex_replace(value.get<std::string>(), std::regex(R"(\\)"), R"(\\)"));

			break;

		case JsonVariantTypeEnum::jBool:
			result += value.get<bool>() ? "true" : "false";

			break;

		case JsonVariantTypeEnum::jInt:
			result += std::to_string(value.get<int64_t>());

			break;

		case JsonVariantTypeEnum::jUint:
			result += std::to_string(value.get<uint64_t>());

			break;

		case JsonVariantTypeEnum::jDouble:
			result += std::format("{:f}", value.get<double>());

			break;

		case JsonVariantTypeEnum::jJSONArray:
		{
			JsonArrayWrapper jsonData(value, &offset);
			const std::vector<JsonObject>& jsonArray = *jsonData;

			if (!jsonData.getOffset())
			{
				throw std::runtime_error("JsonArrayWrapper offset was nullptr");
			}

			result += "[\n";

			std::string& offset = *jsonData.getOffset();

			for (size_t i = 0; i < jsonArray.size(); i++)
			{
				result += offset;

				result += outputJsonType(jsonArray[i], i + 1 == jsonArray.size(), offset);
			}

			result.append(std::string_view(offset.begin(), offset.end() - 2)).append("]");
		}

		break;

		case JsonVariantTypeEnum::jJSONObject:
		{
			JsonObject::ConstIterator begin = value.begin();
			JsonObject::ConstIterator end = value.end();

			result += "{\n";

			while (begin != end)
			{
				JsonObject::ConstIterator check = begin;
				const JsonObject& temp = *check;

				if (std::optional<std::string_view> key = check.key())
				{
					result += std::format(R"({}"{}": )", offset, *key);
				}
				else
				{
					result += offset;
				}

				result += outputJsonType(temp, ++check == end, offset);

				++begin;
			}

			result.append(std::string_view(offset.begin(), offset.end() - 2)).append("}");
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
			result += ',';
		}

		return result.append("\n");
	}

	std::ostream& operator << (std::ostream& outputStream, JsonArrayWrapper jsonData)
	{
		outputStream << "[\n";

		const std::vector<JsonObject>& jsonArray = *jsonData;

		if (!jsonData.getOffset())
		{
			throw std::runtime_error("JsonArrayWrapper offset was nullptr");
		}

		std::string& offset = *jsonData.getOffset();

		for (size_t i = 0; i < jsonArray.size(); i++)
		{
			outputStream << offset;

			outputJsonType(outputStream, jsonArray[i], i + 1 == jsonArray.size(), offset);
		}

		return outputStream;
	}

	std::ostream& operator << (std::ostream& outputStream, const JsonObject& jsonValue)
	{
		std::string offset;

		outputJsonType(outputStream, jsonValue, true, offset);

		return outputStream;
	}
}
