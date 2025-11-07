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
	struct JsonMapHash
	{
		using is_transparent = void;

		size_t operator ()(const char* data) const
		{
			return std::hash<std::string_view>()(data);
		}

		size_t operator ()(std::string_view data) const
		{
			return std::hash<std::string_view>()(data);
		}

		size_t operator ()(const std::string& data) const
		{
			return std::hash<std::string_view>()(data);
		}
	};

	template<typename... Ts>
	struct VisitHelper : Ts...
	{
		using Ts::operator()...;
	};

	template<typename... Ts>
	VisitHelper(Ts...) -> VisitHelper<Ts...>;

	/// <summary>
	/// enum for variantType template arguments
	/// </summary>
	enum class JsonVariantTypeEnum
	{
		jNull,
		jString,
		jBool,
		jInt,
		jUint,
		jDouble,
		jJSONArray,
		jJSONObject
	};

	/**
	 * @brief Custom hash for std::string_view access
	 * @tparam TJsonStruct 
	 */
	template<typename TJsonStruct>
	using JsonMapType = std::unordered_map<std::string, TJsonStruct, JsonMapHash, std::equal_to<>>;

	/// <summary>
	/// Describes all JSON types
	/// </summary>
	template<typename TJsonStruct>
	using JsonVariantType = std::variant
		<
		std::nullptr_t,
		std::string,
		bool,
		int64_t,
		uint64_t,
		double,
		std::vector<TJsonStruct>,
		JsonMapType<TJsonStruct>
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
	constexpr bool operator ==(size_t index, JsonVariantTypeEnum value);

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

	/**
	 * @brief Convert string from sourceCodePage to resultCodePage
	 * @param source String to convert
	 * @param sourceCodePage Source code page
	 * @param resultCodePage Result code page
	 * @return String in resultCodePage
	 */
	std::string convertEncoding(std::string_view source, std::string_view sourceCodePage, std::string_view resultCodePage);
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

	/**
	 * @brief Convert string from sourceCodePage to resultCodePage
	 * @param source String to convert
	 * @param sourceCodePage Source code page
	 * @param resultCodePage Result code page
	 * @return String in resultCodePage
	 */
	std::string convertEncoding(std::string_view source, uint32_t sourceCodePage, uint32_t resultCodePage);
#endif
	/// @brief Get current version of JSON project
	/// @return Current version of JSON project
	std::string getJSONVersion();
}

namespace json::utility
{
	inline constexpr bool operator ==(size_t index, JsonVariantTypeEnum value)
	{
		return index == static_cast<size_t>(value);
	}
}
