#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>
#include <concepts>

#include "CodePageConstants.h"

#ifdef JSON_DLL
#if defined(__LINUX__) || defined(__ANDROID__)
#define JSON_API __attribute__((visibility("default")))
#else
#define JSON_API __declspec(dllexport)
#endif

#define JSON_API_FUNCTION extern "C" JSON_API

#pragma warning(disable: 4297)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4190)
#else
#define JSON_API
#define JSON_API_FUNCTION
#endif // JSON_DLL

namespace json::utility
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
		jUInt64_t,
		jDouble,
		jJSONArray,
		jJSONObject
	};

	/// <summary>
	/// Describes all JSON types
	/// </summary>
	template<typename TJsonStruct>
	using baseVariantType = std::variant
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

	template<typename T, typename TJsonStruct>
	concept JsonValues = std::integral<T> || std::floating_point<T> || std::same_as<T, std::string> || std::same_as<T, std::nullptr_t> || std::same_as<T, std::vector<TJsonStruct>> || std::same_as<T, TJsonStruct>;

	/// @brief JSON object
	class JSON_API jsonObject
	{
	public:
		using variantType = baseVariantType<jsonObject>;

	public:
		/// @brief Iterator through jsonObject
		class JSON_API ConstJSONIterator
		{
		public:
			using ConstJSONIteratorType = std::vector<std::pair<std::string, variantType>>::const_iterator;

		private:
			ConstJSONIteratorType begin;
			ConstJSONIteratorType end;
			ConstJSONIteratorType current;

		public:
			ConstJSONIterator() = default;

			ConstJSONIterator(ConstJSONIteratorType begin, ConstJSONIteratorType end, ConstJSONIteratorType start);

			ConstJSONIterator(const ConstJSONIterator& other) = default;

			ConstJSONIterator(ConstJSONIterator&& other) noexcept = default;

			ConstJSONIterator& operator = (const ConstJSONIterator& other) = default;

			ConstJSONIterator& operator = (ConstJSONIterator&& other) noexcept = default;

			const ConstJSONIteratorType& getBegin() const;

			const ConstJSONIteratorType& getEnd() const;

			ConstJSONIterator operator++(int) noexcept;

			const ConstJSONIterator& operator++() noexcept;

			ConstJSONIterator operator--(int) noexcept;

			const ConstJSONIterator& operator--() noexcept;

			const std::pair<std::string, variantType>& operator*() const noexcept;

			const ConstJSONIteratorType& operator->() const noexcept;

			bool operator==(const ConstJSONIterator& other) const noexcept;

			bool operator!=(const ConstJSONIterator& other) const noexcept;

			operator ConstJSONIteratorType () const;

			~ConstJSONIterator() = default;
		};

	private:
		template<typename T>
		jsonObject& setValue(std::string_view key, T&& value);

		template<typename T>
		bool tryGetValue(std::string_view key, T& value) const;

		ConstJSONIterator::ConstJSONIteratorType findValue(std::string_view key, bool throwException = true) const;

		void appendData(const std::string& key, const json::utility::jsonObject::variantType& value);

	public:
		std::vector<std::pair<std::string, variantType>> data;

	public:
		jsonObject() = default;

		/// @brief Copy constructor
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		jsonObject(const jsonObject& other);

		/// @brief Move constructor
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		jsonObject(jsonObject&& other) noexcept;

		/// @brief Copy assignment operator
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		/// @return Self
		jsonObject& operator=(const jsonObject& other);

		/// @brief Move assignment operator
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		/// @return Self
		jsonObject& operator=(jsonObject&& other) noexcept;

		/// @brief Set null value with given key
		/// @param key JSON key
		jsonObject& setNull(std::string_view key);

		/// @brief Set string value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setString(std::string_view key, std::string_view value);

		/// @brief Set bool value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setBool(std::string_view key, bool value);

		/// @brief Set int value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setInt(std::string_view key, int64_t value);

		/// @brief Set unsigned int value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setUnsignedInt(std::string_view key, uint64_t value);

		/// @brief Set double value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setDouble(std::string_view key, double value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setArray(std::string_view key, const std::vector<jsonObject>& value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setArray(std::string_view key, std::vector<jsonObject>&& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setObject(std::string_view key, const jsonObject& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		jsonObject& setObject(std::string_view key, jsonObject&& value);

		/// @brief Get null value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		std::nullptr_t getNull(std::string_view key) const;

		/// @brief Get string value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::string& getString(std::string_view key) const;

		/// @brief Get bool value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool getBool(std::string_view key) const;

		/// @brief Get int64_t value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		int64_t getInt(std::string_view key) const;

		/// @brief Get uint64_t value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		uint64_t getUnsignedInt(std::string_view key) const;

		/// @brief Get double value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		double getDouble(std::string_view key) const;

		/// @brief Get JSON array. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::vector<jsonObject>& getArray(std::string_view key) const;

		/// @brief Get JSON object. Find and get value only for this JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const jsonObject& getObject(std::string_view key) const;

		/// @brief Try get null value
		/// @param key JSON key
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetNull(std::string_view key) const;

		/// @brief Try get string value
		/// @param key JSON key
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetString(std::string_view key, std::string& value) const;

		/// @brief Try get bool value
		/// @param key JSON key
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetBool(std::string_view key, bool& value) const;

		/// @brief Try get int64_t value
		/// @param key JSON key
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetInt(std::string_view key, int64_t& value) const;

		/// @brief Try get uint64_t value
		/// @param key JSON key
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetUnsignedInt(std::string_view key, uint64_t& value) const;

		/// @brief Try get double value
		/// @param key JSON key
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetDouble(std::string_view key, double& value) const;

		/// @brief Try get JSON array
		/// @param key JSON key
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetArray(std::string_view key, std::vector<utility::jsonObject>& value) const;

		/// @brief Try get JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetObject(std::string_view key, utility::jsonObject& value) const;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param type Object type
		bool contains(std::string_view key, utility::variantTypeEnum type) const;

		/**
		 * @brief Begin iterator
		 * @return
		*/
		ConstJSONIterator begin() const noexcept;

		/**
		 * @brief End iterator
		 * @return
		*/
		ConstJSONIterator end() const noexcept;

		/**
		 * @brief Access JSON value
		 * @param key
		 * @return
		*/
		variantType& operator[](std::string_view key);

		/**
		 * @brief Access JSON value
		 * @param key
		 * @return
		*/
		const variantType& operator[](std::string_view key) const;

		~jsonObject() = default;
	};

	/// @brief Check current iterator with begin or end iterator
	/// @param iterator jsonObject::ConstJSONIterator
	/// @param nestedIterator jsonObject::ConstJSONIterator::getBegin() or jsonObject::ConstJSONIterator::getEnd()
	/// @return 
	bool operator==(const jsonObject::ConstJSONIterator& iterator, const jsonObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator);

	/// @brief Compare index from variant with enum
	/// @param index Index from variant
	/// @param value Value from variantTypeEnum
	/// @return 
	constexpr bool operator==(size_t index, variantTypeEnum value);

#if defined(__LINUX__) || defined(__ANDROID__)
	/// <summary>
	/// Encode string to UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="sourceCodePage">source encoding</param>
	/// <returns>string in UTF8 encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	JSON_API_FUNCTION std::string toUTF8JSON(std::string_view source, std::string_view sourceCodePage);

	/// <summary>
	/// Decode string from UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="resultCodePage">decoding code page</param>
	/// <returns>string in resultCodePage encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	JSON_API_FUNCTION std::string fromUTF8JSON(std::string_view source, std::string_view resultCodePage);
#else
	/// <summary>
	/// Encode string to UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="sourceCodePage">source encoding from https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers</param>
	/// <returns>string in UTF8 encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	JSON_API_FUNCTION std::string toUTF8JSON(std::string_view source, uint32_t sourceCodePage);

	/// <summary>
	/// Decode string from UTF8
	/// </summary>
	/// <param name="source">string to convert</param>
	/// <param name="resultCodePage">decoding code page from https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers</param>
	/// <returns>string in resultCodePage encoding</returns>
	/// <exception cref="json::exceptions::WrongEncodingException"></exception>
	JSON_API_FUNCTION std::string fromUTF8JSON(std::string_view source, uint32_t resultCodePage);
#endif
	/// <summary>
	/// Set to outputStream JSON value
	/// </summary>
	/// <typeparam name="jsonStructT">last argument in baseVariantType</typeparam>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="value">JSON value</param>
	/// <param name="isLast">is description ends</param>
	JSON_API_FUNCTION void outputJSONType(std::ostream& outputStream, const jsonObject::variantType& value, bool isLast, std::string& offset);

	/// @brief Append jsonObject::variantType value to array
	/// @param value JSON value
	/// @param jsonArray Modifiable array
	JSON_API_FUNCTION void appendArray(jsonObject::variantType&& value, std::vector<jsonObject>& jsonArray);

	/// @brief Get current version of JSON project
	/// @return Current version of JSON project
	JSON_API_FUNCTION std::string getJSONVersion();
}

inline bool json::utility::operator==(const jsonObject::ConstJSONIterator& iterator, const jsonObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator)
{
	return static_cast<jsonObject::ConstJSONIterator::ConstJSONIteratorType>(iterator) == nestedIterator;
}

inline constexpr bool json::utility::operator==(size_t index, variantTypeEnum value)
{
	return index == static_cast<size_t>(value);
}
