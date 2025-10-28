#pragma once

#include <stack>

#include "JsonParser.h"

namespace json
{
	class JSON_API RecursiveIterator
	{
	private:
		JsonObject object;
		std::stack<JsonObject::ConstIterator> depth;

	private:
		RecursiveIterator() = default;

	public:
		RecursiveIterator(const JsonParser& parser);

		RecursiveIterator(const JsonObject& object);

		RecursiveIterator operator ++ (int) noexcept;

		const RecursiveIterator& operator ++ () noexcept;

		const std::pair<std::string, JsonObject::VariantType>& operator* () const noexcept;

		JsonObject::ConstIterator::ConstIteratorType operator-> () const noexcept;

		bool operator == (const RecursiveIterator& other) const noexcept;

		bool operator != (const RecursiveIterator& other) const noexcept;

		friend RecursiveIterator end(const RecursiveIterator& it) noexcept;

		~RecursiveIterator() = default;
	};

	inline RecursiveIterator begin(const RecursiveIterator& it) noexcept
	{
		return it;
	}

	inline RecursiveIterator end(const RecursiveIterator&) noexcept
	{
		return RecursiveIterator();
	}
}
