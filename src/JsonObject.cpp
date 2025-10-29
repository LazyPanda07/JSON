#include "JsonObject.h"

#include <algorithm>

#include "Exceptions/CantFindValueException.h"

namespace json
{
	using ConstJSONIterator = JsonObject::ConstIterator;
	using ConstJSONIteratorType = JsonObject::ConstIterator::ConstIteratorType;

	void JsonObject::throwCantFindValueException(std::string_view key)
	{
		throw exceptions::CantFindValueException(key);
	}

	void JsonObject::appendArray(VariantType&& value, std::vector<JsonObject>& jsonArray)
	{
		JsonObject object;

		object.data.emplace_back("", std::move(value));

		jsonArray.push_back(std::move(object));
	}

	ConstJSONIterator::ConstIterator(ConstIteratorType begin, ConstIteratorType end, ConstIteratorType start) :
		begin(begin),
		end(end),
		current(start)
	{

	}

	const ConstJSONIteratorType& ConstJSONIterator::getBegin() const
	{
		return begin;
	}

	const ConstJSONIteratorType& ConstJSONIterator::getEnd() const
	{
		return end;
	}

	ConstJSONIterator ConstJSONIterator::operator++ (int) noexcept
	{
		ConstIterator it(*this);

		++(*this);

		return it;
	}

	const ConstJSONIterator& ConstJSONIterator::operator++ () noexcept
	{
		if (current == end)
		{
			return *this;
		}

		++current;

		return *this;
	}

	ConstJSONIterator ConstJSONIterator::operator-- (int) noexcept
	{
		ConstIterator it(*this);

		--current;

		return it;
	}

	const ConstJSONIterator& ConstJSONIterator::operator-- () noexcept
	{
		if (current == begin)
		{
			return *this;
		}

		--current;

		return *this;
	}

	const std::pair<std::string, JsonObject::VariantType>& ConstJSONIterator::operator* () const noexcept
	{
		return *current;
	}

	const ConstJSONIteratorType& ConstJSONIterator::operator-> () const noexcept
	{
		return current;
	}

	bool ConstJSONIterator::operator == (const ConstIterator& other) const noexcept
	{
		return current == other.current;
	}

	bool ConstJSONIterator::operator != (const ConstIterator& other) const noexcept
	{
		return current != other.current;
	}

	ConstJSONIterator::operator ConstIteratorType () const
	{
		return current;
	}

	template<typename T>
	bool JsonObject::tryGetValue(std::string_view key, T& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		value = std::get<T>(it->second);

		return true;
	}

	std::pair<std::vector<std::pair<std::string, JsonObject::VariantType>>::const_iterator, bool> JsonObject::find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive)
	{
		auto it = std::find_if(start.begin(), start.end(), [&key](const std::pair<std::string, VariantType>& value) { return value.first == key; });
		auto end = start.end();

		if (!recursive || it != end)
		{
			return { it, it != end };
		}

		it = start.begin();

		while (it != end)
		{
			if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONArray))
			{
				const std::vector<JsonObject>& jsonArray = std::get<std::vector<JsonObject>>(it->second);

				for (const JsonObject& object : jsonArray)
				{
					auto result = JsonObject::find(key, object.data, recursive);

					if (result.second)
					{
						return result;
					}
				}
			}
			else if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
			{
				const std::vector<std::pair<std::string, VariantType>>& data = ::std::get<JsonObject>(it->second).data;

				auto result = JsonObject::find(key, data, recursive);

				if (result.second)
				{
					return result;
				}
			}

			++it;
		}

		return { end, false };
	}

	ConstJSONIteratorType JsonObject::findValue(std::string_view key, bool throwException) const
	{
		auto it = find_if(data.begin(), data.end(), [key](const auto& value) { return value.first == key; });

		if (throwException && it == data.end())
		{
			throw exceptions::CantFindValueException(key);
		}

		return it;
	}

	void JsonObject::appendData(const std::string& key, const json::JsonObject::VariantType& value)
	{
		switch (static_cast<json::utility::VariantTypeEnum>(value.index()))
		{
		case json::utility::VariantTypeEnum::jNull:
			data.emplace_back(key, std::get<nullptr_t>(value));

			break;

		case json::utility::VariantTypeEnum::jString:
			data.emplace_back(key, std::get<std::string>(value));

			break;

		case json::utility::VariantTypeEnum::jBool:
			data.emplace_back(key, std::get<bool>(value));

			break;

		case json::utility::VariantTypeEnum::jInt64_t:
			data.emplace_back(key, std::get<int64_t>(value));

			break;

		case json::utility::VariantTypeEnum::jUInt64_t:
			data.emplace_back(key, std::get<uint64_t>(value));

			break;

		case json::utility::VariantTypeEnum::jDouble:
			data.emplace_back(key, std::get<double>(value));

			break;

		case json::utility::VariantTypeEnum::jJSONArray:
			data.emplace_back(key, std::get<std::vector<JsonObject>>(value));

			break;

		case json::utility::VariantTypeEnum::jJSONObject:
			data.emplace_back(key, std::get<JsonObject>(value));

			break;

		default:
			break;
		}
	}

	JsonObject::JsonObject(const JsonObject& other)
	{
		(*this) = other;
	}

	JsonObject::JsonObject(JsonObject&& other) noexcept
	{
		(*this) = std::move(other);
	}

	JsonObject& JsonObject::operator = (const JsonObject& other)
	{
		if (this == &other)
		{
			return *this;
		}

		data.clear();

		for (const auto& [key, value] : other.data)
		{
			this->appendData(key, value);
		}

		return *this;
	}

	JsonObject& JsonObject::operator = (JsonObject&& other) noexcept
	{
		data = std::move(other.data);

		return *this;
	}

	bool JsonObject::contains(std::string_view key, utility::VariantTypeEnum type) const
	{
		return std::any_of(data.begin(), data.end(), [&key, &type](const std::pair<std::string, JsonObject::VariantType>& data) { return data.first == key && data.second.index() == static_cast<size_t>(type); });
	}

	ConstJSONIterator JsonObject::begin() const noexcept
	{
		return ConstIterator(data.cbegin(), data.cend(), data.cbegin());
	}

	ConstJSONIterator JsonObject::end() const noexcept
	{
		return ConstIterator(data.cbegin(), data.cend(), data.cend());
	}

	JsonObject::VariantType& JsonObject::operator[](std::string_view key)
	{
		for (auto& [jsonKey, value] : data)
		{
			if (jsonKey == key)
			{
				return value;
			}
		}

		throw exceptions::CantFindValueException(key);
	}

	const JsonObject::VariantType& JsonObject::operator[](std::string_view key) const
	{
		for (const auto& [jsonKey, value] : data)
		{
			if (jsonKey == key)
			{
				return value;
			}
		}

		throw exceptions::CantFindValueException(key);
	}
}
