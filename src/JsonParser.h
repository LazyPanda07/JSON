#pragma once

#include <istream>

#include "JsonObject.h"

#include "Exceptions/CantFindValueException.h"
#include "Exceptions/WrongInputStreamException.h"

namespace json
{
	/// @brief Parser for JSON
	class JsonParser
	{
	public:
		/// @brief std::variant specialization for JSON
		using VariantType = JsonObject::VariantType;
		
		/// @brief JSON object
		using ObjectType = JsonObject;

		/// @brief Iterator for jsonObject
		using ConstJSONIterator = JsonObject::ConstIterator;

	private:
		std::string rawData;
		JsonObject parsedData;

	private:
		static JsonObject::VariantType parseValue(const std::string& value);

		static void insertKeyValueData(std::string&& key, const std::string& value, JsonObject& ptr);

		static std::pair<std::vector<std::pair<std::string, VariantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive);

		static bool isStringSymbol(char symbol);

		static char interpretEscapeSymbol(char symbol);

		template<utility::JsonValues<JsonObject> T>
		static bool checkSameType(const VariantType& value);

		template<std::integral T>
		static T getValue(const VariantType& value);

	private:
		void parse();

	public:
		JsonParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		JsonParser(std::string_view data);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JsonParser(std::istream& inputStream);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JsonParser(std::istream&& inputStream);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JsonBuilder or JsonParser
		JsonParser(const JsonObject& data);

		/// @brief Copy constructor
		/// @param other Other JsonParser
		JsonParser(const JsonParser& other);

		/// @brief Move constructor
		/// @param other Other JsonParser
		JsonParser(JsonParser&& other) noexcept;

		/// @brief Copy operator
		/// @param other Other JsonParser
		/// @return Self
		JsonParser& operator = (const JsonParser& other);

		/// @brief Move operator
		/// @param other Other JsonParser
		JsonParser& operator = (JsonParser&& other) noexcept;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param recursive Recursive search
		/// @param type Object type
		bool contains(std::string_view key, utility::VariantTypeEnum type, bool recursive = false) const;

#if defined(__LINUX__) || defined(__ANDROID__)
		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted string</param>
		/// <param name="codePage">codePage of jsonData</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		void setJSONData(std::string_view jsonData, std::string_view codePage);
#else
		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted string</param>
		/// <param name="codePage">codePage of jsonData</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		void setJSONData(std::string_view jsonData, uint32_t codePage);
#endif

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(std::string_view jsonData);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(std::string&& jsonData);

		/// @brief Setter for rawData
		/// @param inputStream Stream that contains JSON formatted UTF8 string
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		void setJSONData(std::istream& inputStream);

		/// @brief Setter for rawData
		/// @param inputStream Stream that contains JSON formatted UTF8 string
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		void setJSONData(std::istream&& inputStream);

		/// <summary>
		/// Getter for rawData
		/// </summary>
		/// <returns>rawData</returns>
		const std::string& getRawData() const;

		/// <summary>
		/// Getter for rawData
		/// </summary>
		/// <returns>rawData</returns>
		const std::string& operator * () const;

		ConstJSONIterator begin() const noexcept;

		ConstJSONIterator end() const noexcept;

		/// @brief Getter for parsedData
		/// @return parsedData
		const JsonObject& getParsedData() const;

		/// @brief Move parsed data
		/// @param object Result of moving
		void getParsedData(JsonObject& object) noexcept;

		/**
		 * @brief Override existing value
		 * @param key JSON Key
		 * @param value JSON value
		 * @param recursive Recursive search
		 */
		void overrideValue(std::string_view key, const VariantType& value, bool recursive = false);

		/**
		 * @brief Override existing value
		 * @param key JSON Key
		 * @param value JSON value
		 * @param recursive Recursive search
		 */
		void overrideValue(std::string_view key, VariantType&& value, bool recursive = false);

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JsonParser::jsonStruct::variantType template parameters</typeparam>
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
		/// <typeparam name="T">T is one of JsonParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<utility::JsonHeavyValues<JsonObject> T>
		const T& get(std::string_view key, bool recursive = false) const;

		/**
		 * @brief Getter for all JSON parsed values
		 * @tparam T Is one of json::utility::jsonObject::variantType template parameters
		 * @param key JSON key
		 * @param value JSON value
		 * @param recursive Recursive search
		 * @return True if value found
		*/
		template<utility::JsonValues<JsonObject> T>
		bool tryGet(std::string_view key, T& value, bool recursive = false) const;

		/// <summary>
		/// Get JSON from input stream
		/// </summary>
		/// <param name="stream">std::istream subclass instance</param>
		/// <param name="parser">reference to JsonParser instance</param>
		/// <returns>inputStream</returns>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		friend std::istream& operator >> (std::istream& inputStream, JsonParser& parser);

		/// <summary>
		/// Set JSON to output stream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass instance</param>
		/// <param name="parser">const reference to JsonParser instance</param>
		/// <returns>outputStream</returns>
		friend std::ostream& operator << (std::ostream& outputStream, const JsonParser& parser);

		~JsonParser() = default;
	};

	template<utility::JsonValues<JsonObject> T>
	bool JsonParser::checkSameType(const VariantType& value)
	{
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, std::nullptr_t> || std::is_same_v<T, std::string> || std::is_same_v<T, std::vector<JsonObject>> || std::is_same_v<T, JsonObject>)
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
	T JsonParser::getValue(const VariantType& value)
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
	T JsonParser::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}
		
		const JsonObject::VariantType& value = result->second;

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
			return JsonParser::getValue<T>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonHeavyValues<JsonObject> T>
	const T& JsonParser::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}
		
		const JsonObject::VariantType& value = result->second;

		if constexpr (std::is_same_v<T, std::string>)
		{
			return std::get<std::string>(value);
		}
		else if constexpr (std::is_same_v<T, std::vector<JsonObject>>)
		{
			return std::get<std::vector<JsonObject>>(value);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			return std::get<JsonObject>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonParser::tryGet(std::string_view key, T& value, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success || !JsonParser::checkSameType<T>(result->second))
		{
			return false;
		}

		const JsonObject::VariantType& temp = result->second;

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
		else if constexpr (std::is_same_v<T, std::vector<JsonObject>>)
		{
			value = std::get<std::vector<JsonObject>>(temp);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			value = std::get<JsonObject>(temp);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			value = static_cast<T>(std::get<double>(temp));
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			value = JsonParser::getValue<T>(temp);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return true;
	}
}
