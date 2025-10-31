#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>
#include <concepts>
#include <regex>
#include <algorithm>
#include <string>

#include "CodePageConstants.h"

namespace json::utility
{
	/// <summary>
	/// enum for variantType template arguments
	/// </summary>
	enum class VariantTypeEnum
	{
		jNull,
		jString,
		jBool,
		jInt64_t,
		jUInt64_t,
		jDouble,
		jJSONArray,
		jJSONObject
	};

	/// <summary>
	/// Describes all JSON types
	/// </summary>
	template<typename TJsonStruct>
	using BaseVariantType = std::variant
		<
		std::nullptr_t,
		std::string,
		bool,
		int64_t,
		uint64_t,
		double,
		std::vector<TJsonStruct>,
		TJsonStruct
		>;

	template<typename T>
	concept JsonLightValues = std::integral<T> || std::floating_point<T> || std::same_as<T, std::nullptr_t>;

	template<typename T, typename TJsonStruct>
	concept JsonHeavyValues = std::same_as<T, std::string> || std::same_as<T, std::vector<TJsonStruct>> || std::same_as<T, TJsonStruct>;

	template<typename T, typename TJsonStruct>
	concept JsonValues = JsonLightValues<std::remove_cvref_t<T>> || JsonHeavyValues<std::remove_cvref_t<T>, TJsonStruct>;

	/// @brief Compare index from variant with enum
	/// @param index Index from variant
	/// @param value Value from variantTypeEnum
	/// @return 
	constexpr bool operator ==(size_t index, VariantTypeEnum value);

#if defined(__LINUX__) || defined(__ANDROID__)
	/// <summary>
	/// Encode string to UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="sourceCodePage">source encoding</param>
	/// <returns>string in UTF8 encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	std::string toUTF8JSON(std::string_view source, std::string_view sourceCodePage);

	/// <summary>
	/// Decode string from UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="resultCodePage">decoding code page</param>
	/// <returns>string in resultCodePage encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	std::string fromUTF8JSON(std::string_view source, std::string_view resultCodePage);
#else
	/// <summary>
	/// Encode string to UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="sourceCodePage">source encoding from https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers</param>
	/// <returns>string in UTF8 encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	std::string toUTF8JSON(std::string_view source, uint32_t sourceCodePage);

	/// <summary>
	/// Decode string from UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="resultCodePage">decoding code page from https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers</param>
	/// <returns>string in resultCodePage encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	std::string fromUTF8JSON(std::string_view source, uint32_t resultCodePage);

	std::string convertEncoding(std::string_view source, uint32_t sourceCodePage, uint32_t resultCodePage);
#endif
	/// @brief Get current version of JSON project
	/// @return Current version of JSON project
	std::string getJSONVersion();

	/// <summary>
	/// Put JSON value into outputStream
	/// </summary>
	/// <typeparam name="jsonStructT">last argument in baseVariantType</typeparam>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="value">JSON value</param>
	/// <param name="isLast">is description ends</param>
	template<typename T, typename ArrayWrapperT>
	void outputJsonType(std::ostream& outputStream, const typename T::VariantType& value, bool isLast, std::string& offset);

	namespace __internal
	{
		template<typename VariantType>
		std::pair<typename std::vector<std::pair<std::string, VariantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive);

		template<typename JsonObjectT, utility::JsonValues<JsonObjectT> T, typename VariantType>
		bool checkSameType(const VariantType& value);

		template<std::integral T, typename VariantType>
		T getValue(const VariantType& value);
	}
}

namespace json::utility
{
	inline constexpr bool operator ==(size_t index, VariantTypeEnum value)
	{
		return index == static_cast<size_t>(value);
	}

	template<typename T, typename ArrayWrapperT>
	inline void outputJsonType(std::ostream& outputStream, const typename T::VariantType& value, bool isLast, std::string& offset)
	{
		VariantTypeEnum type = static_cast<VariantTypeEnum>(value.index());

		if (type >= VariantTypeEnum::jJSONArray)
		{
			offset += "  ";
		}

		switch (type)
		{
		case VariantTypeEnum::jNull:
			outputStream << "null";

			break;

		case VariantTypeEnum::jString:
			outputStream << '"' << std::regex_replace(std::get<std::string>(value), std::regex(R"(\\)"), R"(\\)") << '"';

			break;

		case VariantTypeEnum::jBool:
			outputStream << std::boolalpha << std::get<bool>(value);

			break;

		case VariantTypeEnum::jInt64_t:
			outputStream << std::get<int64_t>(value);

			break;

		case VariantTypeEnum::jUInt64_t:
			outputStream << std::get<uint64_t>(value);

			break;

		case VariantTypeEnum::jDouble:
			outputStream << std::fixed << std::get<double>(value);

			break;

		case VariantTypeEnum::jJSONArray:
			outputStream << ArrayWrapperT(value, &offset) << std::string(offset.begin(), offset.end() - 2) << ']';

			break;

		case VariantTypeEnum::jJSONObject:
		{
			const auto& ref = std::get<static_cast<size_t>(VariantTypeEnum::jJSONObject)>(value);

			auto start = ref.data.begin();
			auto end = ref.data.end();

			outputStream << "{\n";

			while (start != end)
			{
				auto check = start;

				if (start->first.size())
				{
					outputStream << offset << '"' << start->first << '"' << ": ";
				}
				else
				{
					outputStream << offset;
				}

				outputJsonType<T, ArrayWrapperT>(outputStream, start->second, ++check == end, offset);

				++start;
			}

			outputStream << std::string(offset.begin(), offset.end() - 2) << '}';
		}

		break;
		}

		if (type >= VariantTypeEnum::jJSONArray)
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

	namespace __internal
	{
		template<typename VariantType>
		inline std::pair<typename std::vector<std::pair<std::string, VariantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive)
		{
			auto it = std::find_if(start.begin(), start.end(), [&key](const std::pair<std::string, VariantType>& value) { return value.first == key; });
			auto end = start.end();

			if (!recursive || it != end)
			{
				return { it, it != end };
			}

			it = start.begin();

			while (it != end)
			{
				if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONArray))
				{
					const auto& jsonArray = std::get<static_cast<size_t>(utility::VariantTypeEnum::jJSONArray)>(it->second);

					for (const auto& object : jsonArray)
					{
						auto result = find(key, object.data, recursive);

						if (result.second)
						{
							return result;
						}
					}
				}
				else if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
				{
					const std::vector<std::pair<std::string, VariantType>>& data = ::std::get<static_cast<size_t>(utility::VariantTypeEnum::jJSONObject)>(it->second).data;

					auto result = find(key, data, recursive);

					if (result.second)
					{
						return result;
					}
				}

				++it;
			}

			return { end, false };
		}

		template<typename JsonObjectT, utility::JsonValues<JsonObjectT> T, typename VariantType>
		inline bool checkSameType(const VariantType& value)
		{
			if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, std::nullptr_t> || std::is_same_v<T, std::string> || std::is_same_v<T, std::vector<JsonObjectT>> || std::is_same_v<T, JsonObjectT>)
			{
				return std::holds_alternative<T>(value);
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				return std::holds_alternative<double>(value);
			}
			else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
			{
				return std::holds_alternative<uint64_t>(value) || std::holds_alternative<int64_t>(value);
			}

			return false;
		}

		template<std::integral T, typename VariantType>
		inline T getValue(const VariantType& value)
		{
			if (std::holds_alternative<int64_t>(value))
			{
				return static_cast<T>(std::get<int64_t>(value));
			}
			else if (std::holds_alternative<uint64_t>(value))
			{
				return static_cast<T>(std::get<uint64_t>(value));
			}

			throw std::runtime_error("Wrong type");

			return T();
		}
	}
}
