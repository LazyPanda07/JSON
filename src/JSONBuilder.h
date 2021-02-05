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
		utility::jsonBuilderStruct builderData;

	public:
		JSONBuilder() = default;

		template<typename T>
		void push_back(const std::pair<std::string, T>& value);

		template<typename T>
		void push_back(std::pair<std::string, T>&& value) noexcept;

		variantType& operator [] (const std::string& key);

		const variantType& operator [] (const std::string& key) const;

		~JSONBuilder() = default;
	};
}
