#pragma once

#include "JsonObject.h"

namespace json::utility
{
	/// @brief Wrapper around array of jsonObjects for simplify some operation
	class JsonArrayWrapper
	{
	private:
		const std::vector<JsonObject>& array;
		std::string* offset;

	public:
		explicit JsonArrayWrapper(const std::vector<JsonObject>& array, std::string* offset = nullptr);

		explicit JsonArrayWrapper(const JsonObject& array, std::string* offset = nullptr);

		size_t size() const;

		const JsonObject& at(size_t index) const;

		/// @brief Get offset for output
		/// @return 
		const std::string* getOffset() const;

		/// @brief Get offset for output
		/// @return 
		std::string* getOffset();

		/// @brief Get reference to wrapped array
		/// @return array
		const std::vector<JsonObject>& operator *() const;

		/// @brief Get jsonObject at given index
		/// @param index Array index
		/// @return 
		/// @exception std::out_of_range 
		const JsonObject& operator [](size_t index) const;

		template<typename T>
		std::vector<T> as() const;

		~JsonArrayWrapper() = default;
	};
}

namespace json::utility
{
	template<typename T>
	std::vector<T> JsonArrayWrapper::as() const
	{
		std::vector<T> result;

		result.reserve(array.size());
		
		for (const JsonObject& object : array)
		{
			result.emplace_back(object.get<T>());
		}

		return result;
	}
}
