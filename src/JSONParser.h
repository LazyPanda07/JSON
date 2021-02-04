#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>

namespace json
{
	class JSONParser
	{
	public:
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
		/// Utility struct
		/// </summary>
		struct jsonStruct
		{
			/// <summary>
			/// Describes all JSON types
			/// </summary>
			using variantType = std::variant
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

			std::unordered_map<std::string, variantType> data;
		};

	public:
		class ConstIterator
		{
		private:
			using ConstIteratorType = std::unordered_map<std::string, jsonStruct::variantType>::const_iterator;

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
		jsonStruct parsedData;

	private:
		template<typename T>
		static void insertDataIntoArray(const std::string& key, T&& value, jsonStruct*& ptr);

		static void insertData(std::string&& key, const std::string& value, jsonStruct*& ptr);

		static std::pair<std::unordered_map<std::string, jsonStruct::variantType>::const_iterator, bool> find(const std::string& key, const std::unordered_map<std::string, jsonStruct::variantType>& start);

		static void outputJSONType(std::ostream& outputStream, const jsonStruct::variantType& value, bool isLast);

	private:
		void parse();

	public:
		JSONParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		JSONParser(const std::string& data);

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
		/// <para>T is one of JSONParser::jsonStruct::variantType template parameters</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="std::runtime_error">can't find JSON value</exception>
		template<typename T>
		const T& get(const std::string& key) const;

		/// <summary>
		/// Get JSON from input stream
		/// </summary>
		/// <param name="stream">std::istream subclass</param>
		/// <param name="parser">reference to JSONParser instance</param>
		/// <returns>stream</returns>
		friend std::istream& operator >> (std::istream& inputStream, JSONParser& parser);

		friend std::ostream& operator << (std::ostream& outputStream, const JSONParser& parser);

		~JSONParser() = default;
	};
}
