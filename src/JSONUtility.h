#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>

#include <Windows.h>

static std::string offset;

namespace json
{
	namespace utility
	{
		/// <summary>
		/// enum for variantType template arguments
		/// </summary>
		enum variantTypeEnum
		{
			jNull,
			jString,
			jChar,
			jUnsignedChar,
			jBool,
			jInt64_t,
			jUint64_t,
			jDouble,
			jNullArray,
			jStringArray,
			jCharArray,
			jUnsignedCharArray,
			jBoolArray,
			jInt64_tArray,
			jUint64_tArray,
			jDoubleArray,
			jJsonStruct
		};

		/// <summary>
		/// Describes all JSON types
		/// </summary>
		template<typename jsonStruct>
		using baseVariantType = std::variant
			<
			nullptr_t,
			std::string,
			char,
			unsigned char,
			bool,
			int64_t,
			uint64_t,
			double,
			std::vector<nullptr_t>,
			std::vector<std::string>,
			std::vector<char>,
			std::vector<unsigned char>,
			std::vector<bool>,
			std::vector<int64_t>,
			std::vector<uint64_t>,
			std::vector<double>,
			std::unique_ptr<jsonStruct>
			>;

		/// <summary>
		/// Utility struct for JSONParser
		/// </summary>
		struct jsonParserStruct
		{
			using variantType = baseVariantType<jsonParserStruct>;

			std::unordered_map<std::string, variantType> data;
		};

		/// <summary>
		/// Utility struct for JSONBuilder
		/// </summary>
		struct jsonBuilderStruct
		{
			using variantType = baseVariantType<jsonBuilderStruct>;

			std::vector<std::pair<std::string, variantType>> data;
		};

		std::string toUTF8JSON(const std::string& source, unsigned int sourceCodepage);

		template<typename jsonStructT>
		void outputJSONType(std::ostream& outputStream, const baseVariantType<jsonStructT>& value, bool isLast);

		template<typename T>
		std::ostream& operator << (std::ostream& outputStream, const std::vector<T>& jsonArray);
	}
}

template<typename jsonStructT>
void json::utility::outputJSONType(std::ostream& outputStream, const json::utility::baseVariantType<jsonStructT>& value, bool isLast)
{
	if (value.index() >= utility::variantTypeEnum::jNullArray)
	{
		offset += "  ";
	}

	switch (value.index())
	{
	case utility::variantTypeEnum::jNull:
		outputStream << "null";

		break;

	case utility::variantTypeEnum::jString:
		outputStream << '"' << std::get<std::string>(value) << '"';

		break;

	case utility::variantTypeEnum::jChar:
		outputStream << std::get<char>(value);

		break;

	case utility::variantTypeEnum::jUnsignedChar:
		outputStream << std::get<unsigned char>(value);

		break;

	case utility::variantTypeEnum::jBool:
		outputStream << std::boolalpha << std::get<bool>(value);

		break;

	case utility::variantTypeEnum::jInt64_t:
		outputStream << std::get<int64_t>(value);

		break;

	case utility::variantTypeEnum::jUint64_t:
		outputStream << std::get<uint64_t>(value);

		break;

	case utility::variantTypeEnum::jDouble:
		outputStream << std::fixed << std::get<double>(value);

		break;

	case utility::variantTypeEnum::jNullArray:
	{
		const std::vector<nullptr_t>& ref = std::get<std::vector<nullptr_t>>(value);

		outputStream << "[\n";

		for (size_t i = 0; i < ref.size(); i++)
		{
			outputStream << offset << "null";

			if (i + 1 != ref.size())
			{
				outputStream << ",\n";
			}
		}

		outputStream << std::string(offset.begin(), offset.end() - 2) << ']';
	}
	break;

	case utility::variantTypeEnum::jStringArray:
		outputStream << std::get<std::vector<std::string>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jCharArray:
		outputStream << std::get<std::vector<char>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jUnsignedCharArray:
		outputStream << std::get<std::vector<unsigned char>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jBoolArray:
		outputStream << std::get<std::vector<bool>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jInt64_tArray:
		outputStream << std::get<std::vector<int64_t>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jUint64_tArray:
		outputStream << std::get<std::vector<uint64_t>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jDoubleArray:
		outputStream << std::fixed << std::get<std::vector<double>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jJsonStruct:
	{
		const std::unique_ptr<jsonStructT>& ref = std::get<std::unique_ptr<jsonStructT>>(value);

		auto start = ref->data.begin();
		auto end = ref->data.end();

		outputStream << "{\n";

		while (start != end)
		{
			auto check = start;

			outputStream << offset << '"' << start->first << '"' << ": ";

			outputJSONType(outputStream, start->second, ++check == end);

			++start;
		}

		outputStream << std::string(offset.begin(), offset.end() - 2) << '}';
	}

	break;
	}

	if (value.index() >= utility::variantTypeEnum::jNullArray)
	{
		offset.pop_back();
		offset.pop_back();
	}

	if (!isLast)
	{
		outputStream << ',';
	}

	outputStream << std::endl;
}

template<typename T>
std::ostream& json::utility::operator << (std::ostream& outputStream, const std::vector<T>& jsonArray)
{
	outputStream << "[\n";

	for (size_t i = 0; i < jsonArray.size(); i++)
	{
		if constexpr (std::is_same_v<std::string, T>)
		{
			outputStream << std::fixed << std::boolalpha << offset << '"' << jsonArray[i] << '"';
		}
		else
		{
			outputStream << std::fixed << std::boolalpha << offset << jsonArray[i];
		}

		if (i + 1 != jsonArray.size())
		{
			outputStream << ',';
		}

		outputStream << std::endl;
	}

	return outputStream;
}
