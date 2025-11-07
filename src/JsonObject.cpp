#include "JsonObject.h"

#include "Exceptions/CantFindValueException.h"
#include "OutputOperations.h"

template<typename... Ts>
struct VisitHelper : Ts...
{
	using Ts::operator()...;
};

template<typename... Ts>
VisitHelper(Ts...) -> VisitHelper<Ts...>;

template <typename T = void>
struct PrefixIncrement
{
	using ResultType = decltype(++std::declval<T&>());

	constexpr ResultType operator ()(T& value) const noexcept(noexcept(++value))
	{
		return ++value;
	}
};

template<template<typename IteratorT> typename OperatorT, typename T>
static void callVisit(T& current);

namespace json
{
	JsonObject::Iterator::Iterator(IteratorType begin, IteratorType end, IteratorType current) :
		begin(begin),
		end(end),
		current(current)
	{

	}

	std::optional<std::string_view> JsonObject::Iterator::key() const
	{
		if (std::holds_alternative<MapType::iterator>(current))
		{
			return std::get<MapType::iterator>(current)->first;
		}

		return std::nullopt;
	}

	JsonObject::Iterator JsonObject::Iterator::operator ++(int) noexcept
	{
		Iterator it(*this);

		++(*this);

		return it;
	}

	JsonObject::Iterator& JsonObject::Iterator::operator ++() noexcept
	{
		if (current == end)
		{
			return *this;
		}

		callVisit<PrefixIncrement>(current);

		return *this;
	}

	JsonObject& JsonObject::Iterator::operator *() noexcept
	{
		return std::visit
		(
			VisitHelper
			(
				[](std::vector<JsonObject>::iterator& it) -> JsonObject&
				{
					return *it;
				},
				[](MapType::iterator& it) -> JsonObject&
				{
					return it->second;
				},
				[](std::pair<JsonObject*, size_t>& it) -> JsonObject&
				{
					return *it.first;
				}
			),
			current
		);
	}

	JsonObject* JsonObject::Iterator::operator ->() noexcept
	{
		return &(**this);
	}

	bool JsonObject::Iterator::operator ==(const Iterator& other) const noexcept
	{
		return current == other.current;
	}

	bool JsonObject::Iterator::operator !=(const Iterator& other) const noexcept
	{
		return current != other.current;
	}

	JsonObject::ConstIterator::ConstIterator(IteratorType begin, IteratorType end, IteratorType current) :
		begin(begin),
		end(end),
		current(current)
	{

	}

	std::optional<std::string_view> JsonObject::ConstIterator::key() const
	{
		if (std::holds_alternative<MapType::const_iterator>(current))
		{
			return std::get<MapType::const_iterator>(current)->first;
		}

		return std::nullopt;
	}

	JsonObject::ConstIterator JsonObject::ConstIterator::operator ++(int) noexcept
	{
		ConstIterator it(*this);

		++(*this);

		return it;
	}

	const JsonObject::ConstIterator& JsonObject::ConstIterator::operator ++() noexcept
	{
		if (current == end)
		{
			return *this;
		}

		callVisit<PrefixIncrement>(current);

		return *this;
	}

	const JsonObject& JsonObject::ConstIterator::operator *() const noexcept
	{
		return std::visit
		(
			VisitHelper
			(
				[](const std::vector<JsonObject>::const_iterator& it) -> const JsonObject&
				{
					return *it;
				},
				[](const MapType::const_iterator& it) -> const JsonObject&
				{
					return it->second;
				},
				[](const std::pair<const JsonObject*, size_t>& it) -> const JsonObject&
				{
					return *it.first;
				}
			),
			current
		);
	}

	const JsonObject* JsonObject::ConstIterator::operator ->() const noexcept
	{
		return &(**this);
	}

	bool JsonObject::ConstIterator::operator ==(const ConstIterator& other) const noexcept
	{
		return current == other.current;
	}

	bool JsonObject::ConstIterator::operator !=(const ConstIterator& other) const noexcept
	{
		return current != other.current;
	}

	bool JsonObject::compareMaps(const MapType& first, const MapType& second)
	{
		if (first.size() != second.size())
		{
			return false;
		}

		for (const auto& [key, value] : first)
		{
			if (auto it = second.find(key); it == second.end())
			{
				return false;
			}
			else if (!(value == it->second))
			{
				return false;
			}
		}

		return true;
	}

	JsonObject::JsonObject() :
		data(nullptr)
	{

	}

	JsonObject::JsonObject(VariantType&& data) :
		data(std::move(data))
	{

	}

	JsonObject& JsonObject::at(size_t index)
	{
		if (!std::holds_alternative<std::vector<JsonObject>>(data))
		{
			throw std::runtime_error(std::format("Can't get value at index: {} on non array JsonObject", index));
		}

		return std::get<std::vector<JsonObject>>(data).at(index);
	}

	JsonObject& JsonObject::at(std::string_view key)
	{
		if (!std::holds_alternative<MapType>(data))
		{
			throw std::runtime_error(std::format("Can't get value with key: {} on non object JsonObject", key));
		}

		MapType& map = std::get<MapType>(data);

		if (auto it = map.find(key); it != map.end())
		{
			return it->second;
		}

		throw std::runtime_error(std::format("Can't get value with key: {}", key));
	}

	const JsonObject& JsonObject::at(size_t index) const
	{
		if (!std::holds_alternative<std::vector<JsonObject>>(data))
		{
			throw std::runtime_error(std::format("Can't get value at index: {} on non array JsonObject", index));
		}

		return std::get<std::vector<JsonObject>>(data).at(index);
	}

	const JsonObject& JsonObject::at(std::string_view key) const
	{
		if (!std::holds_alternative<MapType>(data))
		{
			throw std::runtime_error(std::format("Can't get value with key: {} on non object JsonObject", key));
		}

		const MapType& map = std::get<MapType>(data);

		if (auto it = map.find(key); it != map.end())
		{
			return it->second;
		}

		throw std::runtime_error(std::format("Can't get value with key: {}", key));
	}

	const std::type_info& JsonObject::getType() const
	{
		return std::visit([](auto&& value) -> const std::type_info& { return typeid(value); }, data);
	}

	utility::JsonVariantTypeEnum JsonObject::getEnumType() const
	{
		return static_cast<utility::JsonVariantTypeEnum>(data.index());
	}

	JsonObject::Iterator JsonObject::begin() noexcept
	{
		if (this->is<JsonObject>())
		{
			MapType& map = std::get<MapType>(data);

			return Iterator(map.begin(), map.end(), map.begin());
		}
		else if (this->is<std::vector<JsonObject>>())
		{
			std::vector<JsonObject>& array = std::get<std::vector<JsonObject>>(data);

			return Iterator(array.begin(), array.end(), array.begin());
		}
		else
		{
			std::pair<JsonObject*, size_t> temp(this, 0);

			return Iterator(temp, std::make_pair<JsonObject*, size_t>(this, 1), temp);
		}
	}

	JsonObject::Iterator JsonObject::end() noexcept
	{
		if (this->is<JsonObject>())
		{
			MapType& map = std::get<MapType>(data);

			return Iterator(map.begin(), map.end(), map.end());
		}
		else if (this->is<std::vector<JsonObject>>())
		{
			std::vector<JsonObject>& array = std::get<std::vector<JsonObject>>(data);

			return Iterator(array.begin(), array.end(), array.end());
		}
		else
		{
			std::pair<JsonObject*, size_t> temp(this, 1);

			return Iterator(std::make_pair<JsonObject*, size_t>(this, 0), temp, temp);
		}
	}

	JsonObject::ConstIterator JsonObject::begin() const noexcept
	{
		if (this->is<JsonObject>())
		{
			const MapType& map = std::get<MapType>(data);

			return ConstIterator(map.begin(), map.end(), map.begin());
		}
		else if (this->is<std::vector<JsonObject>>())
		{
			const std::vector<JsonObject>& array = std::get<std::vector<JsonObject>>(data);

			return ConstIterator(array.begin(), array.end(), array.begin());
		}
		else
		{
			std::pair<const JsonObject*, size_t> temp(this, 0);

			return ConstIterator(temp, std::make_pair<const JsonObject*, size_t>(this, 1), temp);
		}
	}

	JsonObject::ConstIterator JsonObject::end() const noexcept
	{
		if (this->is<JsonObject>())
		{
			const MapType& map = std::get<MapType>(data);

			return ConstIterator(map.begin(), map.end(), map.end());
		}
		else if (this->is<std::vector<JsonObject>>())
		{
			const std::vector<JsonObject>& array = std::get<std::vector<JsonObject>>(data);

			return ConstIterator(array.begin(), array.end(), array.end());
		}
		else
		{
			std::pair<const JsonObject*, size_t> temp(this, 1);

			return ConstIterator(std::make_pair<const JsonObject*, size_t>(this, 0), temp, temp);
		}
	}

	bool JsonObject::operator ==(const JsonObject& other) const noexcept
	{
		ConstIterator start = this->begin();
		ConstIterator end = this->end();
		ConstIterator otherStart = other.begin();
		ConstIterator otherEnd = other.end();

		while (start != end)
		{
			if (otherStart == otherEnd)
			{
				return false;
			}

			std::optional<std::string_view> key = start.key();
			std::optional<std::string_view> otherKey = otherStart.key();

			if (key && otherKey)
			{
				return JsonObject::compareMaps(std::get<MapType>(data), std::get<MapType>(other.data));
			}

			if (key != otherKey)
			{
				return false;
			}

			const JsonObject& value = *start;
			const JsonObject& otherValue = *otherStart;

			if (value.getEnumType() != otherValue.getEnumType())
			{
				return false;
			}

			if (value.is<JsonObject>())
			{
				if (!(value == otherValue))
				{
					return false;
				}
			}
			else if (value.is<std::vector<JsonObject>>())
			{
				const std::vector<JsonObject>& array = value.get<std::vector<JsonObject>>();
				const std::vector<JsonObject>& otherArray = otherValue.get<std::vector<JsonObject>>();

				if (array.size() != otherArray.size())
				{
					return false;
				}

				if (!std::equal(array.begin(), array.end(), otherArray.begin(), otherArray.end()))
				{
					return false;
				}
			}
			else
			{
				if (value.data != otherValue.data)
				{
					return false;
				}
			}

			++start;
			++otherStart;
		}

		return true;
	}

	const JsonObject& JsonObject::operator [](size_t index) const
	{
		if (!std::holds_alternative<std::vector<JsonObject>>(data))
		{
			throw std::runtime_error(std::format("Can't get value at index: {} on non array JsonObject", index));
		}

		return std::get<std::vector<JsonObject>>(data)[index];
	}

	const JsonObject& JsonObject::operator [](std::string_view key) const
	{
		if (!std::holds_alternative<MapType>(data))
		{
			throw std::runtime_error(std::format("Can't get value with key: {} on non object JsonObject", key));
		}

		const MapType& map = std::get<MapType>(data);

		if (auto it = map.find(key); it != map.end())
		{
			return it->second;
		}

		throw std::runtime_error(std::format("Can't get value with key: {}", key));

		return {};
	}

	JsonObject& JsonObject::operator [](size_t index)
	{
		return std::get<std::vector<JsonObject>>(data)[index];
	}

	std::ostream& operator <<(std::ostream& stream, const JsonObject& object)
	{
		std::string offset;

		return utility::outputJsonType(stream, object, true, offset);
	}
}

template<template<typename IteratorT> typename OperatorT, typename T>
void callVisit(T& current)
{
	std::visit
	(
		VisitHelper
		(
			[](std::pair<json::JsonObject*, size_t>& it)
			{
				OperatorT<size_t>()(it.second);
			},
			[](std::pair<const json::JsonObject*, size_t>& it)
			{
				OperatorT<size_t>()(it.second);
			},
			[](auto&& it)
			{
				OperatorT<decltype(it)>()(it);
			}
		),
		current
	);
}
