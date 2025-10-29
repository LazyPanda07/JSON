#pragma once

#include "JsonObject.h"

namespace json
{
	/// @brief Builder for JSON
	class JsonBuilder
	{
	public:
		/// @brief std::variant specialization for JSON
		using VariantType = JsonObject::VariantType;

		/// @brief JSON object
		using ObjectType = JsonObject;

		/// @brief Output type for istream operator
		enum class OutputType
		{
			/// @brief human readable JSON
			standard,
			/// @brief no spaces JSON
			minimize
		};

	private:
		JsonObject builderData;
#if defined(__LINUX__) || defined(__ANDROID__)
		std::string_view codePage;
#else
		uint32_t codePage;
#endif
		OutputType type;

	public:
#if defined(__LINUX__) || defined(__ANDROID__)
		/// <summary>
		/// Construct JsonBuilder
		/// </summary>
		/// <param name="codePage">codePage of your system</param>
		/// <param name="type">value from json::JsonBuilder::outputType</param>
		JsonBuilder(std::string_view codePage, OutputType type = OutputType::standard);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JsonBuilder or JsonParser
		/// @param codePage data's codePage
		JsonBuilder(const JsonObject& data, std::string_view codePage, OutputType type = OutputType::standard);
#else
		/// <summary>
		/// Construct JsonBuilder
		/// </summary>
		/// <param name="codePage">codePage of your system</param>
		/// <param name="type">value from json::JsonBuilder::outputType</param>
		JsonBuilder(uint32_t codePage, OutputType type = OutputType::standard);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JsonBuilder or JsonParser
		/// @param codePage data's codePage
		JsonBuilder(const JsonObject& data, uint32_t codePage, OutputType type = OutputType::standard);
#endif

		/// @brief Copy constructor
		/// @param other Other JsonBuilder
		JsonBuilder(const JsonBuilder& other);

		/// @brief Move constructor
		/// @param other Other JsonBuilder
		JsonBuilder(JsonBuilder&& other) noexcept;

		/// @brief Copy operator
		/// @param other Other JsonBuilder
		/// @return Self
		JsonBuilder& operator = (const JsonBuilder& other);

		/// @brief Move operator
		/// @param other Other JsonBuilder
		/// @return Self 
		JsonBuilder& operator = (JsonBuilder&& other) noexcept;

		/// @brief Add JSON key - value
		/// @tparam T is one of json::utility::jsonBuilderStruct::variantType template parameters
		/// @param key JSON key
		/// @param value JSON value
		/// @return Reference to current JsonBuilder instance
		template<typename T>
		JsonBuilder& append(std::string_view key, T&& value = T()) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>);;

		/**
		 * @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		 * @param key Object name
		 * @param type Object type
		 * @param recursive Recursive search
		 * @return 
		*/
		bool contains(std::string_view key, utility::VariantTypeEnum type, bool recursive = false) const;

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist creates new with this key and return reference to it</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		VariantType& operator [](std::string_view key);

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist throws a json::exceptions::CantFindValueException</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException"></exception>
		const VariantType& operator [](std::string_view key) const;

		/// <summary>
		/// Build JSON formatted string
		/// </summary>
		/// <returns>JSON</returns>
		std::string build() const;

		/// <summary>
		/// Set type to json::JsonBuilder::outputType::standard
		/// </summary>
		void standard();

		/// <summary>
		/// Set type to json::JsonBuilder::outputType::minimize
		/// </summary>
		void minimize();

		/// @brief Getter for builded JSON object
		/// @return JSON object
		const JsonObject& getObject() const;

		/// @brief Move builded JSON object from JsonBuilder
		/// @param object Result of moving
		void getObject(JsonObject& object) noexcept;

		/// <summary>
		/// Set JSON to output stream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass instance</param>
		/// <param name="builder">const reference to JsonBuilder instance</param>
		/// <returns>outputStream</returns>
		friend std::ostream& operator << (std::ostream& outputStream, const JsonBuilder& builder);

		~JsonBuilder() = default;
	};

	template<typename T>
	JsonBuilder& JsonBuilder::append(std::string_view key, T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>)
	{
		builderData.setValue(key, std::forward<T>(value));

		return *this;
	}
}
