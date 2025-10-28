#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>
#include <concepts>

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
	concept JsonValues = JsonLightValues<T> || JsonHeavyValues<T, TJsonStruct>;

	/// @brief JSON object
	class JSON_API JSONObject
	{
	public:
		using VariantType = BaseVariantType<JSONObject>;

	public:
		/// @brief Iterator through jsonObject
		class JSON_API ConstJSONIterator
		{
		public:
			using ConstJSONIteratorType = std::vector<std::pair<std::string, VariantType>>::const_iterator;

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

			const std::pair<std::string, VariantType>& operator*() const noexcept;

			const ConstJSONIteratorType& operator->() const noexcept;

			bool operator==(const ConstJSONIterator& other) const noexcept;

			bool operator!=(const ConstJSONIterator& other) const noexcept;

			operator ConstJSONIteratorType () const;

			~ConstJSONIterator() = default;
		};

	private:
		static std::pair<std::vector<std::pair<std::string, VariantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive);

		template<utility::JsonValues<utility::JSONObject> T>
		static bool checkSameType(const VariantType& value);

		template<std::integral T>
		static T getValue(const VariantType& value);

		static void throwCantFindValueException(std::string_view key);

	private:
		template<typename T>
		JSONObject& setValue(std::string_view key, T&& value);

		template<typename T>
		bool tryGetValue(std::string_view key, T& value) const;

		ConstJSONIterator::ConstJSONIteratorType findValue(std::string_view key, bool throwException = true) const;

		void appendData(const std::string& key, const json::utility::JSONObject::VariantType& value);

	public:
		std::vector<std::pair<std::string, VariantType>> data;

	public:
		JSONObject() = default;

		/// @brief Copy constructor
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		JSONObject(const JSONObject& other);

		/// @brief Move constructor
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		JSONObject(JSONObject&& other) noexcept;

		/// @brief Copy assignment operator
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		/// @return Self
		JSONObject& operator =(const JSONObject& other);

		/// @brief Move assignment operator
		/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
		/// @return Self
		JSONObject& operator =(JSONObject&& other) noexcept;

		/// @brief Set null value with given key
		/// @param key JSON key
		JSONObject& setNull(std::string_view key);

		/// @brief Set string value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setString(std::string_view key, std::string_view value);

		/// @brief Set bool value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setBool(std::string_view key, bool value);

		/// @brief Set int value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setInt(std::string_view key, int64_t value);

		/// @brief Set unsigned int value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setUnsignedInt(std::string_view key, uint64_t value);

		/// @brief Set double value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setDouble(std::string_view key, double value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setArray(std::string_view key, const std::vector<JSONObject>& value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setArray(std::string_view key, std::vector<JSONObject>&& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setObject(std::string_view key, const JSONObject& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		JSONObject& setObject(std::string_view key, JSONObject&& value);

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
		const std::vector<JSONObject>& getArray(std::string_view key) const;

		/// @brief Get JSON object. Find and get value only for this JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const JSONObject& getObject(std::string_view key) const;

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
		bool tryGetArray(std::string_view key, std::vector<utility::JSONObject>& value) const;

		/// @brief Try get JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetObject(std::string_view key, utility::JSONObject& value) const;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param type Object type
		bool contains(std::string_view key, utility::VariantTypeEnum type) const;

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
		VariantType& operator [](std::string_view key);

		/**
		 * @brief Access JSON value
		 * @param key
		 * @return
		*/
		const VariantType& operator [](std::string_view key) const;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<utility::JsonLightValues T>
		T get(std::string_view key, bool recursive = false) const;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<utility::JsonHeavyValues<utility::JSONObject> T>
		const T& get(std::string_view key, bool recursive = false) const;

		/**
		 * @brief Getter for all JSON parsed values
		 * @tparam T Is one of json::utility::jsonObject::variantType template parameters
		 * @param key JSON key
		 * @param value JSON value
		 * @param recursive Recursive search
		 * @return True if value found
		*/
		template<utility::JsonValues<utility::JSONObject> T>
		bool tryGet(std::string_view key, T& value, bool recursive = false) const;

		~JSONObject() = default;
	};

	/// @brief Check current iterator with begin or end iterator
	/// @param iterator jsonObject::ConstJSONIterator
	/// @param nestedIterator jsonObject::ConstJSONIterator::getBegin() or jsonObject::ConstJSONIterator::getEnd()
	/// @return 
	bool operator ==(const JSONObject::ConstJSONIterator& iterator, const JSONObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator);

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
	JSON_API_FUNCTION void outputJSONType(std::ostream& outputStream, const JSONObject::VariantType& value, bool isLast, std::string& offset);

	/// @brief Append jsonObject::variantType value to array
	/// @param value JSON value
	/// @param jsonArray Modifiable array
	JSON_API_FUNCTION void appendArray(JSONObject::VariantType&& value, std::vector<JSONObject>& jsonArray);

	/// @brief Get current version of JSON project
	/// @return Current version of JSON project
	JSON_API_FUNCTION std::string getJSONVersion();
}

inline bool json::utility::operator==(const JSONObject::ConstJSONIterator& iterator, const JSONObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator)
{
	return static_cast<JSONObject::ConstJSONIterator::ConstJSONIteratorType>(iterator) == nestedIterator;
}

inline constexpr bool json::utility::operator==(size_t index, VariantTypeEnum value)
{
	return index == static_cast<size_t>(value);
}

namespace json::utility
{
	template<utility::JsonValues<utility::JSONObject> T>
	bool JSONObject::checkSameType(const VariantType& value)
	{
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, std::nullptr_t> || std::is_same_v<T, std::string> || std::is_same_v<T, std::vector<utility::JSONObject>> || std::is_same_v<T, utility::JSONObject>)
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

	template<std::integral T>
	T JSONObject::getValue(const VariantType& value)
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

		return T{};
	}

	template<utility::JsonLightValues T>
	T JSONObject::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JSONObject::find(key, data, recursive);

		if (!success)
		{
			JSONObject::throwCantFindValueException(key);
		}

		const utility::JSONObject::VariantType& value = result->second;

		if constexpr (std::is_same_v<T, bool>)
		{
			return std::get<bool>(value);
		}
		else if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			return std::get<std::nullptr_t>(value);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return static_cast<T>(std::get<double>(value));
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			return JSONObject::getValue<T>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonHeavyValues<utility::JSONObject> T>
	const T& JSONObject::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JSONObject::find(key, data, recursive);

		if (!success)
		{
			JSONObject::throwCantFindValueException(key);
		}

		const utility::JSONObject::VariantType& value = result->second;

		if constexpr (std::is_same_v<T, std::string>)
		{
			return std::get<std::string>(value);
		}
		else if constexpr (std::is_same_v<T, std::vector<utility::JSONObject>>)
		{
			return std::get<std::vector<utility::JSONObject>>(value);
		}
		else if constexpr (std::is_same_v<T, utility::JSONObject>)
		{
			return std::get<utility::JSONObject>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonValues<utility::JSONObject> T>
	bool JSONObject::tryGet(std::string_view key, T& value, bool recursive) const
	{
		auto [result, success] = JSONObject::find(key, data, recursive);

		if (!success || !JSONObject::checkSameType<T>(result->second))
		{
			return false;
		}

		const utility::JSONObject::VariantType& temp = result->second;

		if constexpr (std::is_same_v<T, bool>)
		{
			value = std::get<bool>(temp);
		}
		else if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			value = std::get<std::nullptr_t>(temp);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			value = std::get<std::string>(temp);
		}
		else if constexpr (std::is_same_v<T, std::vector<utility::JSONObject>>)
		{
			value = std::get<std::vector<utility::JSONObject>>(temp);
		}
		else if constexpr (std::is_same_v<T, utility::JSONObject>)
		{
			value = std::get<utility::JSONObject>(temp);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			value = static_cast<T>(std::get<double>(temp));
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			value = JSONObject::getValue<T>(temp);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return true;
	}
}
