#pragma once

#include "JSONUtility.h"

namespace json
{
	/// @brief Builder for JSON
	class JSON_API JSONBuilder
	{
	public:
		/// @brief std::variant specialization for JSON
		using variantType = utility::jsonObject::variantType;

		/// @brief JSON object
		using objectType = utility::jsonObject;

		/// @brief Output type for istream operator
		enum class outputType
		{
			/// @brief human readable JSON
			standard,
			/// @brief no spaces JSON
			minimize
		};

	private:
		utility::jsonObject builderData;
#ifdef __LINUX__
		std::string_view codePage;
#else
		uint32_t codePage;
#endif
		outputType type;

	public:
#ifdef __LINUX__
		/// <summary>
		/// Construct JSONBuilder
		/// </summary>
		/// <param name="codePage">codePage of your system</param>
		/// <param name="type">value from json::JSONBuilder::outputType</param>
		JSONBuilder(std::string_view codePage, outputType type = outputType::standard);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JSONBuilder or JSONParser
		/// @param codePage data's codePage
		JSONBuilder(const utility::jsonObject& data, std::string_view codePage, outputType type = outputType::standard);
#else
		/// <summary>
		/// Construct JSONBuilder
		/// </summary>
		/// <param name="codePage">codePage of your system</param>
		/// <param name="type">value from json::JSONBuilder::outputType</param>
		JSONBuilder(uint32_t codePage, outputType type = outputType::standard);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JSONBuilder or JSONParser
		/// @param codePage data's codePage
		JSONBuilder(const utility::jsonObject& data, uint32_t codePage, outputType type = outputType::standard);
#endif

		/// @brief Copy constructor
		/// @param other Other JSONBuilder
		JSONBuilder(const JSONBuilder& other);

		/// @brief Move constructor
		/// @param other Other JSONBuilder
		JSONBuilder(JSONBuilder&& other) noexcept;

		/// @brief Copy operator
		/// @param other Other JSONBuilder
		/// @return Self
		JSONBuilder& operator = (const JSONBuilder& other);

		/// @brief Move operator
		/// @param other Other JSONBuilder
		/// @return Self 
		JSONBuilder& operator = (JSONBuilder&& other) noexcept;

		/// <summary>
		/// <para>Add JSON key - value</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <typeparam name="T">T is one of json::utility::jsonBuilderStruct::variantType template parameters</typeparam>
		/// <param name="value"><para>first is JSON key, second is JSON value with T template parameter</para><para>T is one of json::utility::jsonBuilderStruct::variantType template parameters</para></param>
		/// <returns>reference to current JSONBuilder instance</returns>
		template<typename T>
		JSONBuilder& push_back(const std::pair<std::string, T>& value);

		/// <summary>
		/// <para>Add JSON key - value</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <typeparam name="T">T is one of json::utility::jsonBuilderStruct::variantType template parameters</typeparam>
		/// <param name="value"><para>first is JSON key, second is JSON value with T template parameter</para><para>T is one of json::utility::jsonBuilderStruct::variantType template parameters</para></param>
		/// <returns>reference to current JSONBuilder instance</returns>
		template<typename T>
		JSONBuilder& push_back(std::pair<std::string, T>&& value) noexcept;

		/// @brief Add JSON key - value
		/// @tparam T is one of json::utility::jsonBuilderStruct::variantType template parameters
		/// @param key JSON key
		/// @param value JSON value
		/// @return Reference to current JSONBuilder instance
		template<typename T>
		JSONBuilder& append(std::string_view key, T&& value);

		/// @brief Add JSON key - null
		/// @param key JSON key
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendNull(std::string_view key);

		/// @brief Add JSON key - string
		/// @param key JSON key
		/// @param value Current codePage encoded value
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendString(std::string_view key, std::string_view value);

		/// @brief Add JSON key - boolean
		/// @param key JSON key
		/// @param value boolean
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendBool(std::string_view key, bool value);

		/// @brief Add JSON key - integer
		/// @param key JSON key
		/// @param value Integer
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendInt(std::string_view key, int64_t value);

		/// @brief Add JSON key - unsigned integer
		/// @param key JSON key
		/// @param value Unsigned integer
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendUnsignedInt(std::string_view key, uint64_t value);

		/**
		 * @brief Add JSON key - double
		 * @param key JSON key
		 * @param value Double
		 * @return Reference to current JSONBuilder instance
		*/
		JSONBuilder& appendDouble(std::string_view key, double value);

		/// @brief Add JSON key - JSON array
		/// @param key JSON key
		/// @param value JSON array
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendArray(std::string_view key, std::vector<utility::jsonObject>&& value);

		/// @brief Add JSON key - JSON array
		/// @param key JSON key
		/// @param value JSON array
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendArray(std::string_view key, const std::vector<utility::jsonObject>& value);

		/// @brief Add JSON key - JSON object
		/// @param key JSON key
		/// @param value JSON object
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendObject(std::string_view key, utility::jsonObject&& value);

		/// @brief Add JSON key - JSON object
		/// @param key JSON key
		/// @param value JSON object
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendObject(std::string_view key, const utility::jsonObject& value);

		/**
		 * @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		 * @param key Object name
		 * @param type Object type
		 * @param recursive Recursive search
		 * @return 
		*/
		bool contains(std::string_view key, utility::variantTypeEnum type, bool recursive = false) const;

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist creates new with this key and return reference to it</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		variantType& operator [] (std::string_view key);

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist throws a json::exceptions::CantFindValueException</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException"></exception>
		const variantType& operator [] (std::string_view key) const;

		/// <summary>
		/// Build JSON formatted string
		/// </summary>
		/// <returns>JSON</returns>
		std::string build() const;

		/// <summary>
		/// Set type to json::JSONBuilder::outputType::standard
		/// </summary>
		void standard();

		/// <summary>
		/// Set type to json::JSONBuilder::outputType::minimize
		/// </summary>
		void minimize();

		/// @brief Getter for builded JSON object
		/// @return JSON object
		const utility::jsonObject& getObject() const;

		/// @brief Move builded JSON object from JSONBuilder
		/// @param object Result of moving
		void getObject(utility::jsonObject& object) noexcept;

		/// <summary>
		/// Set JSON to output stream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass instance</param>
		/// <param name="builder">const reference to JSONBuilder instance</param>
		/// <returns>outputStream</returns>
		friend JSON_API std::ostream& operator << (std::ostream& outputStream, const JSONBuilder& builder);

		~JSONBuilder() = default;
	};

	template<typename T>
	JSONBuilder& JSONBuilder::append(std::string_view key, T&& value)
	{
		return this->push_back(std::make_pair(std::string(key.data(), key.size()), std::forward<decltype(value)>(value)));
	}
}
