#pragma once

#include <stack>

#include "JsonParser.h"

namespace json
{
	class JSON_API RecursiveJSONIterator
	{
	private:
		JsonObject object;
		std::stack<JsonObject::ConstJSONIterator> depth;

	private:
		RecursiveJSONIterator() = default;

	public:
		RecursiveJSONIterator(const JsonParser& parser);

		RecursiveJSONIterator(const JsonObject& object);

		RecursiveJSONIterator operator ++ (int) noexcept;

		const RecursiveJSONIterator& operator ++ () noexcept;

		const std::pair<std::string, JsonObject::VariantType>& operator* () const noexcept;

		JsonObject::ConstJSONIterator::ConstJSONIteratorType operator-> () const noexcept;

		bool operator == (const RecursiveJSONIterator& other) const noexcept;

		bool operator != (const RecursiveJSONIterator& other) const noexcept;

		friend RecursiveJSONIterator end(const RecursiveJSONIterator& it) noexcept;

		~RecursiveJSONIterator() = default;
	};

	inline RecursiveJSONIterator begin(const RecursiveJSONIterator& it) noexcept
	{
		return it;
	}

	inline RecursiveJSONIterator end(const RecursiveJSONIterator&) noexcept
	{
		return RecursiveJSONIterator();
	}
}
