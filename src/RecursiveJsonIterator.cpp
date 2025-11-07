#include "RecursiveJsonIterator.h"

namespace json
{
	void RecursiveJsonIterator::fillIterators(const JsonObject& object)
	{
		JsonObject::ConstIterator begin = object.begin();
		JsonObject::ConstIterator end = object.end();

		iterators.push(std::make_pair(begin, end));

		while (begin != end)
		{
			const JsonObject& value = *begin;

			if (value.is<JsonObject>() || value.is<std::vector<JsonObject>>())
			{
				this->fillIterators(value);
			}

			++begin;
		}
	}

	RecursiveJsonIterator::RecursiveJsonIterator(const JsonParser& parser)
	{
		this->fillIterators(parser.getParsedData());
	}

	RecursiveJsonIterator::RecursiveJsonIterator(const JsonBuilder& builder)
	{
		this->fillIterators(builder.getObject());
	}

	RecursiveJsonIterator::RecursiveJsonIterator(const JsonObject& object)
	{
		this->fillIterators(object);
	}

	RecursiveJsonIterator RecursiveJsonIterator::begin() const
	{
		return *this;
	}

	RecursiveJsonIterator RecursiveJsonIterator::end() const
	{
		return RecursiveJsonIterator();
	}

	RecursiveJsonIterator RecursiveJsonIterator::operator++ (int) noexcept
	{
		RecursiveJsonIterator it(*this);

		++(*this);

		return it;
	}

	const RecursiveJsonIterator& RecursiveJsonIterator::operator++ () noexcept
	{
		if (iterators.empty())
		{
			return *this;
		}

		auto& [current, end] = iterators.top();

		do
		{
			++current;

			if (current == end)
			{
				break;
			}
		} while (current->is<JsonObject>() || current->is<std::vector<JsonObject>>());

		if (current == end)
		{
			iterators.pop();
		}

		return *this;
	}

	const JsonObject::ConstIterator& RecursiveJsonIterator::operator* () const noexcept
	{
		return iterators.top().first;
	}

	const JsonObject::ConstIterator* RecursiveJsonIterator::operator-> () const noexcept
	{
		return &iterators.top().first;
	}

	bool RecursiveJsonIterator::operator ==(const RecursiveJsonIterator& other) const noexcept
	{
		if (iterators.empty())
		{
			return true;
		}

		return false;
	}
}
