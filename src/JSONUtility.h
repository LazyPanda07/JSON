#pragma once

#include <unordered_map>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <sstream>

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif // !CP_UTF8

#ifdef JSON_DLL
#define JSON_API __declspec(dllexport)
#define JSON_API_FUNCTION extern "C" __declspec(dllexport)

#pragma warning(disable: 4297)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4190)
#else
#define JSON_API
#define JSON_API_FUNCTION
#endif // JSON_DLL

namespace json
{
	namespace utility
	{
		/// <summary>
		/// enum for variantType template arguments
		/// </summary>
		enum class variantTypeEnum
		{
			jNull,
			jString,
			jBool,
			jInt64_t,
			jUInt64_t,
			jDouble,
			jJSONArray,
			jJSONObject
		};

		/// <summary>
		/// Describes all JSON types
		/// </summary>
		template<typename jsonStruct>
		using baseVariantType = std::variant
			<
			nullptr_t,
			std::string,
			bool,
			int64_t,
			uint64_t,
			double,
			std::vector<jsonStruct>,
			jsonStruct
			>;

		/// @brief JSON object
		struct JSON_API jsonObject
		{
		public:
			using variantType = baseVariantType<jsonObject>;

		public:
			/// @brief Iterator through jsonObject
			class JSON_API ConstJSONIterator
			{
			public:
				using ConstJSONIteratorType = std::vector<std::pair<std::string, variantType>>::const_iterator;

			private:
				ConstJSONIteratorType begin;
				ConstJSONIteratorType end;
				ConstJSONIteratorType current;

			public:
				ConstJSONIterator() = default;

				ConstJSONIterator(const ConstJSONIterator& other);

				ConstJSONIterator(ConstJSONIteratorType begin, ConstJSONIteratorType end, ConstJSONIteratorType start);

				const ConstJSONIteratorType& getBegin() const;

				const ConstJSONIteratorType& getEnd() const;

				ConstJSONIterator operator++(int) noexcept;

				const ConstJSONIterator& operator++() noexcept;

				ConstJSONIterator operator--(int) noexcept;

				const ConstJSONIterator& operator--() noexcept;

				const std::pair<std::string, variantType>& operator*() const noexcept;

				const ConstJSONIteratorType& operator->() const noexcept;

				bool operator==(const ConstJSONIterator& other) const noexcept;

				bool operator!=(const ConstJSONIterator& other) const noexcept;

				operator ConstJSONIteratorType () const;

				~ConstJSONIterator() = default;
			};

		public:
			std::vector<std::pair<std::string, variantType>> data;

			jsonObject() = default;

			/// @brief Copy constructor
			/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
			jsonObject(const jsonObject& other);

			/// @brief Move constructor
			/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
			jsonObject(jsonObject&& other) noexcept;

			/// @brief Copy assignment operator
			/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
			/// @return Self
			jsonObject& operator=(const jsonObject& other);

			/// @brief Move assignment operator
			/// @param other Another jsonObject from JSONParser or JSONBuilder or custom
			/// @return Self
			jsonObject& operator=(jsonObject&& other) noexcept;

			/// @brief Get null value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return nullptr value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			nullptr_t getNull(const std::string& key) const;

			/// @brief Get string value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return string value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			const std::string& getString(const std::string& key) const;

			/// @brief Get bool value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return bool value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			bool getBool(const std::string& key) const;

			/// @brief Get int64_t value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return int64_t value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			int64_t getInt(const std::string& key) const;

			/// @brief Get uint64_t value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return uint64_t value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			uint64_t getUnsignedInt(const std::string& key) const;

			/// @brief Get double value. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return double value
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			double getDouble(const std::string& key) const;

			/// @brief Get JSON array. Find and get value only for this JSON object
			/// @param key JSON key
			/// @return JSON array
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			const std::vector<jsonObject>& getArray(const std::string& key) const;

			/// @brief Get JSON object. Find and get value only for this JSON object
			/// @param key JSON Key
			/// @return JSON object
			/// @exception json::exceptions::CantFindValueException 
			/// @exception std::bad_variant_access Other type found
			const jsonObject& getObject(const std::string& key) const;

			/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
			/// @param key Object name
			/// @param type Object type
			bool contains(const std::string& key, utility::variantTypeEnum type) const;

			ConstJSONIterator begin() const noexcept;

			ConstJSONIterator end() const noexcept;
		};

		/// @brief Check current iterator with begin or end iterator
		/// @param iterator jsonObject::ConstJSONIterator
		/// @param nestedIterator jsonObject::ConstJSONIterator::getBegin() or jsonObject::ConstJSONIterator::getEnd()
		/// @return 
		bool operator== (const jsonObject::ConstJSONIterator& iterator, const jsonObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator);

		/// <summary>
		/// Encode string to UTF8
		/// </summary>
		/// <param name="source">string to convert</param>
		/// <param name="sourceCodePage">source encoding</param>
		/// <returns>string in UTF8 encoding</returns>
		/// <exception cref="json::exceptions::WrongEncodingException"></exception>
		JSON_API_FUNCTION std::string toUTF8JSON(const std::string& source, uint32_t sourceCodePage);

		/// <summary>
		/// Decode string from UTF8
		/// </summary>
		/// <param name="source">string to convert</param>
		/// <param name="resultCodePage">decoding code page</param>
		/// <returns>string in resultCodePage encoding</returns>
		/// <exception cref="json::exceptions::WrongEncodingException"></exception>
		JSON_API_FUNCTION std::string fromUTF8JSON(const std::string& source, uint32_t resultCodePage);

		/// <summary>
		/// Set to outputStream JSON value
		/// </summary>
		/// <typeparam name="jsonStructT">last argument in baseVariantType</typeparam>
		/// <param name="outputStream">std::ostream subclass</param>
		/// <param name="value">JSON value</param>
		/// <param name="isLast">is description ends</param>
		JSON_API_FUNCTION void outputJSONType(std::ostream& outputStream, const jsonObject::variantType& value, bool isLast, std::string& offset);

		/// @brief Append jsonObject::variantType value to array
		/// @param value JSON value
		JSON_API_FUNCTION void appendArray(jsonObject::variantType&& value, std::vector<jsonObject>& jsonArray);

		/// @brief Get current version of JSON project
		/// @return Current version of JSON project
		JSON_API_FUNCTION std::string getJSONVersion();
	}
}

inline bool json::utility::operator== (const jsonObject::ConstJSONIterator& iterator, const jsonObject::ConstJSONIterator::ConstJSONIteratorType& nestedIterator)
{
	return static_cast<jsonObject::ConstJSONIterator::ConstJSONIteratorType>(iterator) == nestedIterator;
}
