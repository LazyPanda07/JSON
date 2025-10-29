#pragma once

#include <format>

#include "JsonUtility.h"

namespace json
{
	/// @brief JSON object
	class JSON_API JsonObject
	{
	public:
		using VariantType = utility::BaseVariantType<JsonObject>;

	public:
		/// @brief Iterator through jsonObject
		class JSON_API ConstIterator
		{
		public:
			using ConstIteratorType = std::vector<std::pair<std::string, VariantType>>::const_iterator;

		private:
			ConstIteratorType begin;
			ConstIteratorType end;
			ConstIteratorType current;

		public:
			ConstIterator() = default;

			ConstIterator(ConstIteratorType begin, ConstIteratorType end, ConstIteratorType start);

			ConstIterator(const ConstIterator& other) = default;

			ConstIterator(ConstIterator&& other) noexcept = default;

			ConstIterator& operator = (const ConstIterator& other) = default;

			ConstIterator& operator = (ConstIterator&& other) noexcept = default;

			const ConstIteratorType& getBegin() const;

			const ConstIteratorType& getEnd() const;

			ConstIterator operator++(int) noexcept;

			const ConstIterator& operator++() noexcept;

			ConstIterator operator--(int) noexcept;

			const ConstIterator& operator--() noexcept;

			const std::pair<std::string, VariantType>& operator*() const noexcept;

			const ConstIteratorType& operator->() const noexcept;

			bool operator==(const ConstIterator& other) const noexcept;

			bool operator!=(const ConstIterator& other) const noexcept;

			operator ConstIteratorType () const;

			~ConstIterator() = default;
		};

	private:
		static std::pair<std::vector<std::pair<std::string, VariantType>>::const_iterator, bool> find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive);

		template<utility::JsonValues<JsonObject> T>
		static bool checkSameType(const VariantType& value);

		template<std::integral T>
		static T getValue(const VariantType& value);

		static void throwCantFindValueException(std::string_view key);

	public:
		/// @brief Append jsonObject::variantType value to array
		/// @param value JSON value
		/// @param jsonArray Modifiable array
		static void appendArray(VariantType&& value, std::vector<JsonObject>& jsonArray);

	private:
		template<typename T>
		bool tryGetValue(std::string_view key, T& value) const;

		ConstIterator::ConstIteratorType findValue(std::string_view key, bool throwException = true) const;

		void appendData(const std::string& key, const json::JsonObject::VariantType& value);

	public:
		std::vector<std::pair<std::string, VariantType>> data;

	public:
		JsonObject() = default;

		/// @brief Copy constructor
		/// @param other Another jsonObject from JsonParser or JsonBuilder or custom
		JsonObject(const JsonObject& other);

		/// @brief Move constructor
		/// @param other Another jsonObject from JsonParser or JsonBuilder or custom
		JsonObject(JsonObject&& other) noexcept;

		/// @brief Copy assignment operator
		/// @param other Another jsonObject from JsonParser or JsonBuilder or custom
		/// @return Self
		JsonObject& operator =(const JsonObject& other);

		/// @brief Move assignment operator
		/// @param other Another jsonObject from JsonParser or JsonBuilder or custom
		/// @return Self
		JsonObject& operator =(JsonObject&& other) noexcept;

		template<typename T>
		JsonObject& setValue(std::string_view key, T&& value = T()) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>);

		/// @brief Set null value with given key
		/// @param key JSON key
		JsonObject& setNull(std::string_view key);

		/// @brief Set string value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setString(std::string_view key, std::string_view value);

		/// @brief Set bool value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setBool(std::string_view key, bool value);

		/// @brief Set int value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setInt(std::string_view key, int64_t value);

		/// @brief Set unsigned int value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setUnsignedInt(std::string_view key, uint64_t value);

		/// @brief Set double value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setDouble(std::string_view key, double value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setArray(std::string_view key, const std::vector<JsonObject>& value);

		/// @brief Set array value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setArray(std::string_view key, std::vector<JsonObject>&& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setObject(std::string_view key, const JsonObject& value);

		/// @brief Set object value with given key
		/// @param key JSON key
		/// @param value JSON value
		JsonObject& setObject(std::string_view key, JsonObject&& value);

		/// @brief Get null value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		std::nullptr_t getNull(std::string_view key) const;

		/// @brief Get string value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::string& getString(std::string_view key) const;

		/// @brief Get bool value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool getBool(std::string_view key) const;

		/// @brief Get int64_t value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		int64_t getInt(std::string_view key) const;

		/// @brief Get uint64_t value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		uint64_t getUnsignedInt(std::string_view key) const;

		/// @brief Get double value. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		double getDouble(std::string_view key) const;

		/// @brief Get JSON array. Find and get value only for this JSON object
		/// @param key JSON key
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::vector<JsonObject>& getArray(std::string_view key) const;

		/// @brief Get JSON object. Find and get value only for this JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const JsonObject& getObject(std::string_view key) const;

		/// @brief Try get null value
		/// @param key JSON key
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetNull(std::string_view key) const;

		/// @brief Try get string value
		/// @param key JSON key
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetString(std::string_view key, std::string& value) const;

		/// @brief Try get bool value
		/// @param key JSON key
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetBool(std::string_view key, bool& value) const;

		/// @brief Try get int64_t value
		/// @param key JSON key
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetInt(std::string_view key, int64_t& value) const;

		/// @brief Try get uint64_t value
		/// @param key JSON key
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetUnsignedInt(std::string_view key, uint64_t& value) const;

		/// @brief Try get double value
		/// @param key JSON key
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetDouble(std::string_view key, double& value) const;

		/// @brief Try get JSON array
		/// @param key JSON key
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetArray(std::string_view key, std::vector<JsonObject>& value) const;

		/// @brief Try get JSON object
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool tryGetObject(std::string_view key, JsonObject& value) const;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param type Object type
		bool contains(std::string_view key, utility::VariantTypeEnum type) const;

		/**
		 * @brief Begin iterator
		 * @return
		*/
		ConstIterator begin() const noexcept;

		/**
		 * @brief End iterator
		 * @return
		*/
		ConstIterator end() const noexcept;

		/**
		 * @brief Access JSON value
		 * @param key
		 * @return
		*/
		VariantType& operator [](std::string_view key);

		/**
		 * @brief Access JSON value
		 * @param key
		 * @return
		*/
		const VariantType& operator [](std::string_view key) const;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JsonParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<utility::JsonLightValues T>
		T get(std::string_view key, bool recursive = false) const;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JsonParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <param name="recursive">Recursive search</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<utility::JsonHeavyValues<JsonObject> T>
		const T& get(std::string_view key, bool recursive = false) const;

		/**
		 * @brief Getter for all JSON parsed values
		 * @tparam T Is one of json::utility::jsonObject::variantType template parameters
		 * @param key JSON key
		 * @param value JSON value
		 * @param recursive Recursive search
		 * @return True if value found
		*/
		template<utility::JsonValues<JsonObject> T>
		bool tryGet(std::string_view key, T& value, bool recursive = false) const;

		~JsonObject() = default;
	};

	namespace utility
	{
		/// @brief Check current iterator with begin or end iterator
		/// @param iterator jsonObject::ConstJSONIterator
		/// @param nestedIterator jsonObject::ConstJSONIterator::getBegin() or jsonObject::ConstJSONIterator::getEnd()
		/// @return 
		bool operator ==(const JsonObject::ConstIterator& iterator, const JsonObject::ConstIterator::ConstIteratorType& nestedIterator);
	}
}

namespace json
{
	namespace utility
	{
		inline bool operator ==(const JsonObject::ConstIterator& iterator, const JsonObject::ConstIterator::ConstIteratorType& nestedIterator)
		{
			return static_cast<JsonObject::ConstIterator::ConstIteratorType>(iterator) == nestedIterator;
		}
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::checkSameType(const VariantType& value)
	{
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, std::nullptr_t> || std::is_same_v<T, std::string> || std::is_same_v<T, std::vector<JsonObject>> || std::is_same_v<T, JsonObject>)
		{
			return std::holds_alternative<T>(value);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return std::holds_alternative<double>(value);
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			return std::holds_alternative<uint64_t>(value) || std::holds_alternative<int64_t>(value);
		}

		return false;
	}

	template<std::integral T>
	T JsonObject::getValue(const VariantType& value)
	{
		if (std::holds_alternative<int64_t>(value))
		{
			return static_cast<T>(std::get<int64_t>(value));
		}
		else if (std::holds_alternative<uint64_t>(value))
		{
			return static_cast<T>(std::get<uint64_t>(value));
		}

		throw std::runtime_error("Wrong type");

		return T{};
	}

	template<typename T>
	JsonObject& JsonObject::setValue(std::string_view key, T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>)
	{
		using ActualT = std::remove_cvref_t<T>;

		if constexpr (std::is_same_v<ActualT, bool>)
		{
			data.emplace_back(key, value);
		}
		else if constexpr (std::is_same_v<ActualT, std::nullptr_t>)
		{
			data.emplace_back(key, nullptr);
		}
		else if constexpr (std::is_same_v<ActualT, std::string>)
		{
			data.emplace_back(key, std::forward<T>(value));
		}
		else if constexpr (std::is_convertible_v<ActualT, std::string>)
		{
			data.emplace_back(key, static_cast<std::string>(value));
		}
		else if constexpr (std::convertible_to<ActualT, std::string_view>)
		{
			data.emplace_back(key, static_cast<std::string_view>(value).data());
		}
		else if constexpr (std::is_same_v<ActualT, std::vector<JsonObject>> || std::is_same_v<ActualT, JsonObject>)
		{
			data.emplace_back(key, std::forward<T>(value));
		}
		else if constexpr (std::is_floating_point_v<ActualT>)
		{
			data.emplace_back(key, static_cast<double>(value));
		}
		else if constexpr (std::is_unsigned_v<ActualT>)
		{
			data.emplace_back(key, static_cast<uint64_t>(value));
		}
		else if constexpr (std::is_signed_v<ActualT>)
		{
			data.emplace_back(key, static_cast<int64_t>(value));
		}
		else
		{
			throw std::invalid_argument(std::format("Wrong argument type: {}", typeid(T).name()));
		}

		return *this;
	}

	template<utility::JsonLightValues T>
	T JsonObject::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JsonObject::find(key, data, recursive);

		if (!success)
		{
			JsonObject::throwCantFindValueException(key);
		}

		const JsonObject::VariantType& value = result->second;

		if constexpr (std::is_same_v<T, bool>)
		{
			return std::get<bool>(value);
		}
		else if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			return std::get<std::nullptr_t>(value);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return static_cast<T>(std::get<double>(value));
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			return JsonObject::getValue<T>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonHeavyValues<JsonObject> T>
	const T& JsonObject::get(std::string_view key, bool recursive) const
	{
		auto [result, success] = JsonObject::find(key, data, recursive);

		if (!success)
		{
			JsonObject::throwCantFindValueException(key);
		}

		const JsonObject::VariantType& value = result->second;

		if constexpr (std::is_same_v<T, std::string>)
		{
			return std::get<std::string>(value);
		}
		else if constexpr (std::is_same_v<T, std::vector<JsonObject>>)
		{
			return std::get<std::vector<JsonObject>>(value);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			return std::get<JsonObject>(value);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::tryGet(std::string_view key, T& value, bool recursive) const
	{
		auto [result, success] = JsonObject::find(key, data, recursive);

		if (!success || !JsonObject::checkSameType<T>(result->second))
		{
			return false;
		}

		const JsonObject::VariantType& temp = result->second;

		if constexpr (std::is_same_v<T, bool>)
		{
			value = std::get<bool>(temp);
		}
		else if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			value = std::get<std::nullptr_t>(temp);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			value = std::get<std::string>(temp);
		}
		else if constexpr (std::is_same_v<T, std::vector<JsonObject>>)
		{
			value = std::get<std::vector<JsonObject>>(temp);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			value = std::get<JsonObject>(temp);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			value = static_cast<T>(std::get<double>(temp));
		}
		else if constexpr (std::is_unsigned_v<T> || std::is_signed_v<T>)
		{
			value = JsonObject::getValue<T>(temp);
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return true;
	}
}
