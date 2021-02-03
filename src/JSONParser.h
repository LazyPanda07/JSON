#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>

namespace json
{
	class JSONParser
	{
	private:
		struct jsonStruct
		{
			std::unordered_map<std::string, std::variant
				<
				nullptr_t,
				std::string,
				char,
				unsigned char,
				bool,
				int,
				unsigned int,
				float,
				int64_t,
				uint64_t,
				double,
				std::vector<nullptr_t>,
				std::vector<std::string>,
				std::vector<char>,
				std::vector<unsigned char>,
				std::vector<bool>,
				std::vector<short>,
				std::vector<unsigned short>,
				std::vector<int>,
				std::vector<unsigned int>,
				std::vector<float>,
				std::vector<int64_t>,
				std::vector<uint64_t>,
				std::vector<double>,
				std::unique_ptr<jsonStruct>
				>
			> data;
		};

	private:
		const std::string rawData;
		jsonStruct parsedData;

	private:
		static void insertKeyValue(std::string&& key, const std::string& value, jsonStruct*& ptr);

		void parse(const std::string& data);

	public:
		JSONParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		JSONParser(const std::string& data);

		~JSONParser() = default;
	};
}
