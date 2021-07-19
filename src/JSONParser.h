#pragma once

#include <fstream>

#include "JSONUtility.h"

namespace json
{
	/// @brief Parser for JSON
	class JSON_API JSONParser
	{
	public:
		/// @brief std::variant specialization for JSONParser
		using variantType = utility::jsonParserStruct::variantType;
		
		/// @brief JSON object for JSONParser
		using objectType = utility::jsonParserStruct;

		/// @brief JSON array for JSONParser
		using arrayType = utility::jsonParserArray;

	public:
		/// @brief Iterator through JSONParser
		class JSON_API ConstIterator
		{
		private:
			using ConstIteratorType = std::unordered_map<std::string, variantType>::const_iterator;

		private:
			ConstIteratorType begin;
			ConstIteratorType end;
			ConstIteratorType current;

		public:
			ConstIterator() = default;

			ConstIterator(const ConstIterator& other);

			ConstIterator(ConstIteratorType begin, ConstIteratorType end, ConstIteratorType start);

			ConstIterator operator ++ (int) noexcept;

			const ConstIterator& operator ++ () noexcept;

			ConstIterator operator -- (int) noexcept;

			const ConstIterator& operator -- () noexcept;

			const ConstIteratorType& operator * () const noexcept;

			const ConstIteratorType& operator -> () const noexcept;

			bool operator == (const ConstIterator& other) const noexcept;

			bool operator != (const ConstIterator& other) const noexcept;

			~ConstIterator() = default;
		};

	private:
		std::string rawData;
		utility::jsonParserStruct parsedData;

	private:
		template<typename T>
		static void insertDataIntoArray(const std::string& key, T&& value, utility::jsonParserStruct*& ptr);

		static void insertData(std::string&& key, const std::string& value, utility::jsonParserStruct*& ptr);

		static std::pair<std::unordered_map<std::string, variantType>::const_iterator, bool> find(const std::string& key, const std::unordered_map<std::string, variantType>& start);

		static bool isStringSymbol(char symbol);

	private:
		void parse();

	public:
		JSONParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		JSONParser(const std::string& data);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		JSONParser(std::ifstream& inputStream);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		JSONParser(std::ifstream&& inputStream);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted string</param>
		/// <param name="codepage">codepage of jsonData</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		void setJSONData(const std::string& jsonData, uint32_t codePage);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(const std::string& jsonData);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(std::string&& jsonData) noexcept;

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

		/// <summary>
		/// Get iterator to begin of the JSON parsedData
		/// </summary>
		/// <returns>begin iterator</returns>
		ConstIterator begin() const noexcept;

		/// <summary>
		/// Get iterator to end of the JSON parsedData
		/// </summary>
		/// <returns>end iterator</returns>
		ConstIterator end() const noexcept;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>T is one of json::utility::jsonParserStruct::variantType template parameters</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		template<typename T>
		const T& get(const std::string& key) const;

		/// <summary>
		/// Get JSON from input stream
		/// </summary>
		/// <param name="stream">std::istream subclass instance</param>
		/// <param name="parser">reference to JSONParser instance</param>
		/// <returns>inputStream</returns>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
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
