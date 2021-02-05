#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>

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
	}
}
