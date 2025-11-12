#include "MapJsonIterator.h"

namespace json
{
	MapJsonIterator::MapJsonIterator(const JsonObject& object)
	{
		if (!object.is<JsonObject>())
		{
			throw std::runtime_error("Can't iterate through non Map object");
		}

		current = object.begin();
		endIt = object.end();
	}

	MapJsonIterator MapJsonIterator::begin() const
	{
		return *this;
	}

	MapJsonIterator MapJsonIterator::end() const
	{
		return *this;
	}

	const MapJsonIterator& MapJsonIterator::operator ++() noexcept
	{
		if (current != endIt)
		{
			++current;
		}

		return *this;
	}

	std::pair<std::string, JsonObject> MapJsonIterator::operator *() const noexcept
	{
		return std::make_pair(std::string(*current.key()), *current);
	}

	bool MapJsonIterator::operator ==(const MapJsonIterator& other) const noexcept
	{
		return current == other.endIt;
	}
}
