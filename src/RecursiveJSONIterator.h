#pragma once

#include <stack>

#include "JSONParser.h"

namespace json
{
	class JSON_API RecursiveJSONIterator
	{
	private:
		utility::JsonObject object;
		std::stack<utility::JsonObject::ConstJSONIterator> depth;

	private:
		RecursiveJSONIterator() = default;

	public:
		RecursiveJSONIterator(const JSONParser& parser);

		RecursiveJSONIterator(const utility::JsonObject& object);

		RecursiveJSONIterator operator ++ (int) noexcept;

		const RecursiveJSONIterator& operator ++ () noexcept;

		const std::pair<std::string, utility::JsonObject::VariantType>& operator* () const noexcept;

		utility::JsonObject::ConstJSONIterator::ConstJSONIteratorType operator-> () const noexcept;

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
