#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif // !CP_UTF8

#ifdef JSON_DLL
#define JSON_API __declspec(dllexport)
#define JSON_API_FUNCTION extern "C" __declspec(dllexport)
#else
#define JSON_API
#define JSON_API_FUNCTION
#endif // JSON_DLL

static std::string offset;

namespace json
{
	namespace utility
	{
		/// <summary>
		/// enum for variantType template arguments
		/// </summary>
		enum class variantTypeEnum
		{
			jNull,
			jString,
			jBool,
			jInt64_t,
			jUint64_t,
			jDouble,
			jJSONArray,
			jJSONObject
		};

		/// <summary>
		/// Describes all JSON types
		/// </summary>
		template<typename jsonStruct>
		using baseVariantType = std::variant
			<
			nullptr_t,
			std::string,
			bool,
			int64_t,
			uint64_t,
			double,
#ifdef JSON_DLL
			std::vector<std::shared_ptr<jsonStruct>>,
			std::shared_ptr<jsonStruct>
#else
			std::vector<std::unique_ptr<jsonStruct>>,
			std::unique_ptr<jsonStruct>
#endif // JSON_DLL>
			> ;

		/// @brief JSON object for JSONParser
		struct JSON_API jsonParserStruct
		{
			using variantType = baseVariantType<jsonParserStruct>;

			std::vector<std::pair<std::string, variantType>> data;
		};

		/// @brief JSON array for JSONParser
		struct JSON_API jsonParserArray
		{
			using variantType = baseVariantType<jsonParserArray>;

			std::vector<variantType> data;
		};

		/// @brief JSON object for JSONBuilder
		struct JSON_API jsonBuilderStruct
		{
			using variantType = baseVariantType<jsonBuilderStruct>;

			std::vector<std::pair<std::string, variantType>> data;
		};

		/// @brief JSON array for JSONBuilder
		struct JSON_API jsonBuilderArray
		{
			using variantType = baseVariantType<jsonBuilderArray>;

			std::vector<variantType> data;
		};

		/// <summary>
		/// Encode string to UTF8
		/// </summary>
		/// <param name="source">string to convert</param>
		/// <param name="sourceCodePage">source encoding</param>
		/// <returns>string in UTF8 encoding</returns>
		/// <exception cref="json::exceptions::WrongEncodingException"></exception>
		JSON_API_FUNCTION std::string toUTF8JSON(const std::string& source, uint32_t sourceCodePage);

		/// <summary>
		/// Decode string from UTF8
		/// </summary>
		/// <param name="source">string to convert</param>
		/// <param name="resultCodePage">decoding code page</param>
		/// <returns>string in resultCodePage encoding</returns>
		/// <exception cref="json::exceptions::WrongEncodingException"></exception>
		JSON_API_FUNCTION std::string fromUTF8JSON(const std::string& source, uint32_t resultCodePage);

		/// <summary>
		/// Set to outputStream JSON value
		/// </summary>
		/// <typeparam name="jsonStructT">last argument in baseVariantType</typeparam>
		/// <param name="outputStream">std::ostream subclass</param>
		/// <param name="value">JSON value</param>
		/// <param name="isLast">is description ends</param>
		template<typename jsonStructT>
		void outputJSONType(std::ostream& outputStream, const baseVariantType<jsonStructT>& value, bool isLast);

		/// <summary>
		/// Output JSON arrays to std::ostream
		/// </summary>
		/// <typeparam name="T">type of JSON array</typeparam>
		/// <param name="outputStream">std::ostream subclass</param>
		/// <param name="jsonArray">JSON array</param>
		/// <returns>outputStream</returns>
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
		outputStream << std::get<std::vector<nullptr_t>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

		break;

	case utility::variantTypeEnum::jStringArray:
		outputStream << std::get<std::vector<std::string>>(value) << std::string(offset.begin(), offset.end() - 2) << ']';

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
#ifdef JSON_DLL
		const std::shared_ptr<jsonStructT>& ref = std::get<std::shared_ptr<jsonStructT>>(value);
#else
		const std::unique_ptr<jsonStructT>& ref = std::get<std::unique_ptr<jsonStructT>>(value);
#endif // JSON_DLL

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
		else if constexpr (std::is_same_v<nullptr_t, T>)
		{
			outputStream << std::fixed << std::boolalpha << offset << "null";
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
