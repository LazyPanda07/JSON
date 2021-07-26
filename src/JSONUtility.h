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

#pragma warning(disable: 4297)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4190)
#else
#define JSON_API
#define JSON_API_FUNCTION
#endif // JSON_DLL

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
			JUInt64_t,
			jDouble,
			jJSONArray,
			jJSONObject
		};

#ifdef JSON_DLL
		/// @brief Using declaration for smart pointers
		/// @tparam T Data type of smart pointer
		template<typename T>
		using objectSmartPointer = std::shared_ptr<T>;
#else
		/// @brief Using declaration for smart pointers
		/// @tparam T Data type of smart pointer
		template<typename T>
		using objectSmartPointer = std::unique_ptr<T>;
#endif // JSON_API

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
			std::vector<objectSmartPointer<jsonStruct>>,
			objectSmartPointer<jsonStruct>
			>;

		/// @brief JSON object
		struct JSON_API jsonObject
		{
			static inline std::string offset;

			using variantType = baseVariantType<jsonObject>;

			std::vector<std::pair<std::string, variantType>> data;
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
		JSON_API_FUNCTION void outputJSONType(std::ostream& outputStream, const jsonObject::variantType& value, bool isLast);

		/// <summary>
		/// Output JSON arrays to std::ostream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass</param>
		/// <param name="jsonData">JSON array</param>
		/// <returns>outputStream</returns>
		JSON_API_FUNCTION std::ostream& operator << (std::ostream& outputStream, const jsonObject::variantType& jsonData);

		/// @brief Append jsonObject::variantType value to array
		/// @param value JSON value
		JSON_API_FUNCTION void appendArray(jsonObject::variantType&& value, std::vector<objectSmartPointer<jsonObject>>& jsonArray);

		/// @brief Make function for objectSmartPointer
		/// @tparam T Data type of smart pointer
		/// @param ...args Construct arguments
		/// @return Newly created objectSmartPointer
		template<typename T, typename... Args>
		objectSmartPointer<T> make_object(Args&&... args);
	}
}

template<typename T, typename... Args>
json::utility::objectSmartPointer<T> json::utility::make_object(Args&&... args)
{
	if constexpr (std::is_same_v<objectSmartPointer<T>, std::unique_ptr<T>>)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	else if constexpr (std::is_same_v<objectSmartPointer<T>, std::shared_ptr<T>>)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
