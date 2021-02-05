#pragma once

#include <sstream>

#include "JSONUtility.h"

namespace json
{
	class JSONBuilder
	{
	public:
		using variantType = utility::jsonBuilderStruct::variantType;

	private:
		static std::pair<std::vector<std::pair<std::string, variantType>>::iterator, bool> find(const std::string& key, std::vector<std::pair<std::string, variantType>>& start);

		static std::pair<std::vector<std::pair<std::string, variantType>>::const_iterator, bool> find(const std::string& key, const std::vector<std::pair<std::string, variantType>>& start);

	private:
		utility::jsonBuilderStruct builderData;

	public:
		JSONBuilder() = default;

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

		~JSONBuilder() = default;
	};
}