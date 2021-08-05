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
		static std::pair<std::vector<std::pair<std::string, variantType>>::iterator, bool> find(const std::string& key, std::vector<std::pair<std::string, variantType>>& start);

		static std::pair<std::vector<std::pair<std::string, variantType>>::const_iterator, bool> find(const std::string& key, const std::vector<std::pair<std::string, variantType>>& start);

	private:
		utility::jsonObject builderData;
		uint32_t codepage;
		outputType type;

	public:
		/// <summary>
		/// Construct JSONBuilder
		/// </summary>
		/// <param name="codepage">codepage of your system</param>
		/// <param name="type">value from json::JSONBuilder::outputType</param>
		JSONBuilder(uint32_t codepage, outputType type = outputType::standard);

		/// @brief Deleted copy constructor
		JSONBuilder(const JSONBuilder&) = delete;

		/// @brief Move constructor
		/// @param other Other JSONBuilder
		JSONBuilder(JSONBuilder&& other) noexcept;

		/// @brief Deleted copy operator 
		JSONBuilder& operator = (const JSONBuilder&) = delete;

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
		JSONBuilder& append(const std::string& key, T&& value);

		/// @brief Add JSON key - value
		/// @tparam T is one of json::utility::jsonBuilderStruct::variantType template parameters
		/// @param key JSON key
		/// @param value JSON value
		/// @return Reference to current JSONBuilder instance
		template<typename T>
		JSONBuilder& append(std::string&& key, T&& value);

		/// @brief Add JSON key - null
		/// @param key JSON key
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendNull(const std::string& key);

		/// @brief Add JSON key - string
		/// @param key JSON key
		/// @param value Current codepage encoded value
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendString(const std::string& key, const std::string& value);

		/// @brief Add JSON key - string
		/// @param key JSON key
		/// @param value Current codepage encoded value
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendString(const std::string& key, std::string&& value);

		/// @brief Add JSON key - integer
		/// @param key JSON key
		/// @param value Integer
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendInt(const std::string& key, int64_t value);

		/// @brief Add JSON key - unsigned integer
		/// @param key JSON key
		/// @param value Unsigned integer
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendUnsignedInt(const std::string& key, uint64_t value);

		/// @brief Add JSON key - JSON array
		/// @param key JSON key
		/// @param value JSON array
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendArray(const std::string& key, std::vector<utility::objectSmartPointer<utility::jsonObject>>&& value);

		/// @brief Add JSON key - JSON object
		/// @param key JSON key
		/// @param value JSON object
		/// @return Reference to current JSONBuilder instance
		JSONBuilder& appendObject(const std::string& key, utility::objectSmartPointer<utility::jsonObject>&& value);

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param type Object type
		bool contains(const std::string& key, utility::variantTypeEnum type);

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist creates new with this key and return reference to it</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		variantType& operator [] (const std::string& key);

		/// <summary>
		/// <para>Access to JSON value operator</para>
		/// <para>If key doesn't exist throws a json::exceptions::CantFindValueException</para>
		/// <para>Always use std::string as parameter or ""s literal</para>
		/// </summary>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException"></exception>
		const variantType& operator [] (const std::string& key) const;

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
	JSONBuilder& JSONBuilder::append(const std::string& key, T&& value)
	{
		return this->push_back(std::make_pair(key, std::forward<decltype(value)>(value)));
	}

	template<typename T>
	JSONBuilder& JSONBuilder::append(std::string&& key, T&& value)
	{
		return this->push_back(std::make_pair(std::move(key), std::forward<decltype(value)>(value)));
	}
}
