#pragma once

#include <istream>

#include "JSONUtility.h"

#include "Exceptions/CantFindValueException.h"
#include "Exceptions/WrongInputStreamException.h"

namespace json
{
	/// @brief Parser for JSON
	class JSON_API JSONParser
	{
	public:
		/// @brief std::variant specialization for JSON
		using variantType = utility::jsonObject::variantType;
		
		/// @brief JSON object
		using objectType = utility::jsonObject;

		/// @brief Iterator for jsonObject
		using ConstJSONIterator = utility::jsonObject::ConstJSONIterator;

	private:
		std::string rawData;
		utility::jsonObject parsedData;

	private:
		static utility::jsonObject::variantType parseValue(const std::string& value);

		static void insertKeyValueData(std::string&& key, const std::string& value, utility::jsonObject& ptr);

		static std::pair<std::vector<std::pair<std::string, variantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, variantType>>& start, bool recursive);

		static bool isStringSymbol(char symbol);

		static char interpretEscapeSymbol(char symbol);

		template<utility::variantTypeEnum ResultType>
		static bool checkDifferType(const variantType& checkType);

	private:
		void parse();

	public:
		JSONParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		JSONParser(std::string_view data);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JSONParser(std::istream& inputStream);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JSONParser(std::istream&& inputStream);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JSONBuilder or JSONParser
		JSONParser(const utility::jsonObject& data);

		/// @brief Copy constructor
		/// @param other Other JSONParser
		JSONParser(const JSONParser& other);

		/// @brief Move constructor
		/// @param other Other JSONParser
		JSONParser(JSONParser&& other) noexcept;

		/// @brief Copy operator
		/// @param other Other JSONParser
		/// @return Self
		JSONParser& operator = (const JSONParser& other);

		/// @brief Move operator
		/// @param other Other JSONParser
		JSONParser& operator = (JSONParser&& other) noexcept;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param recursive Recursive search
		/// @param type Object type
		bool contains(std::string_view key, utility::variantTypeEnum type, bool recursive = false) const;

#ifdef __LINUX__
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
		void setJSONData(std::string&& jsonData) noexcept;

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

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>T is one of json::utility::jsonObject::variantType template parameters</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<typename T>
		const T& getValue(std::string_view key, bool recursive = false) const;

		/**
		 * @brief Getter for all JSON parsed values
		 * @tparam T Is one of json::utility::jsonObject::variantType template parameters
		 * @param key JSON key
		 * @param value JSON value
		 * @param recursive Recursive search
		 * @return True if value found
		*/
		template<typename T>
		bool tryGetValue(std::string_view key, T& value, bool recursive = false) const;

		/// @brief Get null value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		nullptr_t getNull(std::string_view key, bool recursive = false) const;

		/// @brief Get string value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::string& getString(std::string_view key, bool recursive = false) const;
		
		/// @brief Get bool value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool getBool(std::string_view key, bool recursive = false) const;

		/// @brief Get int64_t value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		int64_t getInt(std::string_view key, bool recursive = false) const;

		/// @brief Get uint64_t value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		uint64_t getUnsignedInt(std::string_view key, bool recursive = false) const;

		/// @brief Get double value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		double getDouble(std::string_view key, bool recursive = false) const;

		/// @brief Get JSON array
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::vector<utility::jsonObject>& getArray(std::string_view key, bool recursive = false) const;

		/// @brief Get JSON object
		/// @param key JSON Key
		/// @param recursive Recursive search
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const utility::jsonObject& getObject(std::string_view key, bool recursive = false) const;

		/// @brief Try get null value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetNull(std::string_view key, bool recursive = false) const;

		/// @brief Try get string value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetString(std::string_view key, std::string& value, bool recursive = false) const;

		/// @brief Try get bool value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetBool(std::string_view key, bool& value, bool recursive = false) const;

		/// @brief Try get int64_t value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetInt(std::string_view key, int64_t& value, bool recursive = false) const;

		/// @brief Try get uint64_t value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetUnsignedInt(std::string_view key, uint64_t& value, bool recursive = false) const;

		/// @brief Try get double value
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetDouble(std::string_view key, double& value, bool recursive = false) const;

		/// @brief Try get JSON array
		/// @param key JSON key
		/// @param recursive Recursive search
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetArray(std::string_view key, std::vector<utility::jsonObject>& value, bool recursive = false) const;

		/// @brief Try get JSON object
		/// @param key JSON Key
		/// @param recursive Recursive search
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetObject(std::string_view key, utility::jsonObject& value, bool recursive = false) const;

		/// @brief Getter for parsedData
		/// @return parsedData
		const utility::jsonObject& getParsedData() const;

		/// @brief Move parsed data
		/// @param object Result of moving
		void getParsedData(utility::jsonObject& object) noexcept;

		/// <summary>
		/// Get JSON from input stream
		/// </summary>
		/// <param name="stream">std::istream subclass instance</param>
		/// <param name="parser">reference to JSONParser instance</param>
		/// <returns>inputStream</returns>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		friend JSON_API std::istream& operator >> (std::istream& inputStream, JSONParser& parser);

		/// <summary>
		/// Set JSON to output stream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass instance</param>
		/// <param name="parser">const reference to JSONParser instance</param>
		/// <returns>outputStream</returns>
		friend JSON_API std::ostream& operator << (std::ostream& outputStream, const JSONParser& parser);

		~JSONParser() = default;
	};
}
