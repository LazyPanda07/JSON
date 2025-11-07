#pragma once

#include <stack>

#include "JsonParser.h"
#include "JsonBuilder.h"

namespace json
{
	class RecursiveJsonIterator
	{
	private:
		void fillIterators(const JsonObject& object);

	private:
		std::stack<std::pair<JsonObject::ConstIterator, JsonObject::ConstIterator>> iterators;

	private:
		RecursiveJsonIterator() = default;

	public:
		RecursiveJsonIterator(const JsonParser& parser);

		RecursiveJsonIterator(const JsonBuilder& builder);

		RecursiveJsonIterator(const JsonObject& object);

		RecursiveJsonIterator begin() const;

		RecursiveJsonIterator end() const;

		RecursiveJsonIterator operator ++ (int) noexcept;

		const RecursiveJsonIterator& operator ++ () noexcept;

		const JsonObject::ConstIterator& operator* () const noexcept;

		const JsonObject::ConstIterator* operator-> () const noexcept;

		bool operator ==(const RecursiveJsonIterator& other) const noexcept;

		~RecursiveJsonIterator() = default;
	};
}
