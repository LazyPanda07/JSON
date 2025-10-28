#pragma once

#include "JSONUtility.h"

namespace json
{
	namespace utility
	{
		/// @brief Wrapper around array of jsonObjects for simplify some operation
		class JSON_API JSONArrayWrapper
		{
		private:
			const std::vector<JSONObject>& array;
			std::string* offset;

		public:
			explicit JSONArrayWrapper(const std::vector<JSONObject>& array, std::string* offset = nullptr);

			explicit JSONArrayWrapper(const JSONObject::VariantType& array, std::string* offset = nullptr);

			size_t size() const;

			/// @brief Get value as nullptr
			/// @param index Array index
			/// @return nullptr
			/// @exception std::out_of_range
			std::nullptr_t getNull(size_t index) const;

			/// @brief Get value as string
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range
			std::string getString(size_t index) const;

			/// @brief Get value as bool
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range
			bool getBool(size_t index) const;

			/// @brief Get value as int64_t
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range
			int64_t getInt64_t(size_t index) const;

			/// @brief Get value as uint64_t
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range
			uint64_t getUInt64_t(size_t index) const;

			/// @brief Get value as double
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range
			double getDouble(size_t index) const;

			/// @brief Get value as object
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range 
			const JSONObject& getObject(size_t index) const;

			/// @brief Convert wrapped array into array with nullptr_t values
			/// @return 
			std::vector<std::nullptr_t> getAsNullArray() const;

			/// @brief Convert wrapped array into array with string values
			/// @return 
			std::vector<std::string> getAsStringArray() const;

			/// @brief Convert wrapped array into array with bool values
			/// @return 
			std::vector<bool> getAsBoolArray() const;

			/// @brief Convert wrapped array into array with int64_t values
			/// @return 
			std::vector<int64_t> getAsInt64_tArray() const;

			/// @brief Convert wrapped array into array with uint64_t values
			/// @return 
			std::vector<uint64_t> getAsUInt64_tArray() const;

			/// @brief Convert wrapped array into array with double values
			/// @return 
			std::vector<double> getAsDoubleArray() const;

			/// @brief Convert wrapped array into array with object values
			/// @return 
			std::vector<JSONObject> getAsObjectArray() const;

			/// @brief Get offset for output
			/// @return 
			const std::string* getOffset() const;

			/// @brief Get offset for output
			/// @return 
			std::string* getOffset();

			/// @brief Get reference to wrapped array
			/// @return array
			const std::vector<JSONObject>& operator * () const;

			/// @brief Get jsonObject at given index
			/// @param index Array index
			/// @return 
			/// @exception std::out_of_range 
			const JSONObject& operator [] (size_t index) const;

			~JSONArrayWrapper() = default;
		};
	}
}
