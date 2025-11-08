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
	private:
		JsonObject parsedData;
		std::string rawData;

	private:
		static utility::JsonVariantType<JsonObject> parseValue(const std::string& value);

		static void insertValue(std::string_view key, const std::string& value, JsonObject& object);

		static char interpretEscapeSymbol(char symbol);

	private:
		void parse();

		JsonObject* find(std::string_view key, bool recursive);

		const JsonObject* find(std::string_view key, bool recursive) const;

	public:
		JsonParser() = default;

		JsonParser(std::string_view data);

		JsonParser(std::istream& inputStream);

		JsonParser(std::istream&& inputStream);

		JsonParser(const JsonObject& data);

		JsonParser(const JsonParser& other);

		JsonParser(JsonParser&& other) noexcept;

		JsonParser& operator = (const JsonParser& other);

		JsonParser& operator = (JsonParser&& other) noexcept;

#if defined(__LINUX__) || defined(__ANDROID__)
		void setJSONData(std::string_view jsonData, std::string_view codePage);
#else
		void setJSONData(std::string_view jsonData, uint32_t codePage);
#endif

		void setJSONData(std::string_view jsonData);

		void setJSONData(std::string&& jsonData);

		void setJSONData(std::istream& inputStream);

		void setJSONData(std::istream&& inputStream);

		const std::string& getRawData() const;

		/// <summary>
		/// Getter for rawData
		/// </summary>
		/// <returns>rawData</returns>
		const std::string& operator *() const;

		JsonObject::ConstIterator begin() const noexcept;

		JsonObject::ConstIterator end() const noexcept;

		const JsonObject& getParsedData() const;

		/**
		 * @brief Move parsed JSON object from JsonBuilder
		 * @param object Result of moving
		 */
		void getParsedData(JsonObject& object) noexcept;

		/**
		 * @brief Override existing value
		 * @param key JSON Key
		 * @param value JSON value
		 * @param recursive Recursive search
		 */
		template<typename T>
		void overrideValue(std::string_view key, T&& value, bool recursive = false) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>);

		template<utility::JsonValues<JsonObject> T>
		bool contains(std::string_view key, bool recursive = false) const;

		template<utility::JsonLightValues T>
		T get(std::string_view key, bool recursive = false) const;

		template<utility::JsonHeavyValues<JsonObject> T>
		const T& get(std::string_view key, bool recursive = false) const;

		template<utility::JsonValues<JsonObject> T>
		bool tryGet(std::string_view key, T& value, bool recursive = false) const;

		friend std::istream& operator >> (std::istream& inputStream, JsonParser& parser);

		friend std::ostream& operator << (std::ostream& outputStream, const JsonParser& parser);

		~JsonParser() = default;
	};
}

namespace json
{
	template<typename T>
	void JsonParser::overrideValue(std::string_view key, T&& value, bool recursive) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>)
	{
		JsonObject* result = this->find(key, recursive);

		if (!result)
		{
			throw exceptions::CantFindValueException(key);
		}

		(*result) = value;
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonParser::contains(std::string_view key, bool recursive) const
	{
		return parsedData.contains<T>(key, recursive);
	}

	template<utility::JsonLightValues T>
	T JsonParser::get(std::string_view key, bool recursive) const
	{
		const JsonObject* result = this->find(key, recursive);

		if (!result)
		{
			throw exceptions::CantFindValueException(key);
		}

		return result->get<T>();
	}

	template<utility::JsonHeavyValues<JsonObject> T>
	const T& JsonParser::get(std::string_view key, bool recursive) const
	{
		const JsonObject* result = this->find(key, recursive);

		if (!result)
		{
			throw exceptions::CantFindValueException(key);
		}

		return result->get<T>();
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonParser::tryGet(std::string_view key, T& value, bool recursive) const
	{
		const JsonObject* result = this->find(key, recursive);

		if (!result)
		{
			return false;
		}

		return result->tryGet<T>(value);
	}
}
