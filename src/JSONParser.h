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
	private:
		struct jsonStruct
		{
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
				> ;

			std::unordered_map<std::string, variantType> data;
		};

	private:
		std::string rawData;
		jsonStruct parsedData;

	private:
		template<typename T>
		static void insertDataIntoArray(const std::string& key, T&& value, jsonStruct*& ptr);

		static void insertData(std::string&& key, const std::string& value, jsonStruct*& ptr);

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
		/// Get JSON from stream
		/// </summary>
		/// <param name="stream">std::istream subclass</param>
		/// <param name="parser">reference to JSONParser instance</param>
		/// <returns>stream</returns>
		friend std::istream& operator >> (std::istream& stream, JSONParser& parser);

		~JSONParser() = default;
	};
}
