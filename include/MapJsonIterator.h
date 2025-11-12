#pragma once

#include "JsonObject.h"

namespace json
{
	class MapJsonIterator
	{
	private:
		JsonObject::ConstIterator current;
		JsonObject::ConstIterator endIt;

	public:
		MapJsonIterator(const JsonObject& object);

		MapJsonIterator begin() const;

		MapJsonIterator end() const;

		const MapJsonIterator& operator ++() noexcept;

		std::pair<std::string, JsonObject> operator *() const noexcept;

		bool operator ==(const MapJsonIterator& other) const noexcept;

		~MapJsonIterator() = default;
	};
}
