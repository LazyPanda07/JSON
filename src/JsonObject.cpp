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
	JsonObject& JsonObject::setValue(std::string_view key, T&& value)
	{
		if constexpr (std::is_same_v<std::string_view&, decltype(value)>)
		{
			data.emplace_back(std::string(key.data(), key.size()), std::string(value.data(), value.size()));
		}
		else
		{
			data.emplace_back(std::string(key.data(), key.size()), std::forward<T>(value));
		}

		return *this;
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

	JsonObject& JsonObject::setNull(std::string_view key)
	{
		return this->setValue(key, nullptr);
	}

	JsonObject& JsonObject::setString(std::string_view key, std::string_view value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setBool(std::string_view key, bool value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setInt(std::string_view key, int64_t value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setUnsignedInt(std::string_view key, uint64_t value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setDouble(std::string_view key, double value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setArray(std::string_view key, const std::vector<JsonObject>& value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setArray(std::string_view key, std::vector<JsonObject>&& value)
	{
		return this->setValue(key, std::move(value));
	}

	JsonObject& JsonObject::setObject(std::string_view key, const JsonObject& value)
	{
		return this->setValue(key, value);
	}

	JsonObject& JsonObject::setObject(std::string_view key, JsonObject&& value)
	{
		return this->setValue(key, std::move(value));
	}

	nullptr_t JsonObject::getNull(std::string_view key) const
	{
		return std::get<nullptr_t>(this->findValue(key)->second);
	}

	const std::string& JsonObject::getString(std::string_view key) const
	{
		return std::get<std::string>(this->findValue(key)->second);
	}

	bool JsonObject::getBool(std::string_view key) const
	{
		return std::get<bool>(this->findValue(key)->second);
	}

	int64_t JsonObject::getInt(std::string_view key) const
	{
		auto it = this->findValue(key);

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jUInt64_t:
			return static_cast<int64_t>(std::get<uint64_t>(it->second));

		case json::utility::VariantTypeEnum::jDouble:
			return static_cast<int64_t>(std::get<double>(it->second));
		}

		return std::get<int64_t>(it->second);
	}

	uint64_t JsonObject::getUnsignedInt(std::string_view key) const
	{
		auto it = this->findValue(key);

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jInt64_t:
			return static_cast<uint64_t>(std::get<int64_t>(it->second));

		case json::utility::VariantTypeEnum::jDouble:
			return static_cast<uint64_t>(std::get<double>(it->second));
		}

		return std::get<uint64_t>(it->second);
	}

	double JsonObject::getDouble(std::string_view key) const
	{
		auto it = this->findValue(key);

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jInt64_t:
			return static_cast<double>(std::get<int64_t>(it->second));

		case json::utility::VariantTypeEnum::jUInt64_t:
			return static_cast<double>(std::get<uint64_t>(it->second));
		}

		return std::get<double>(it->second);
	}

	const std::vector<JsonObject>& JsonObject::getArray(std::string_view key) const
	{
		return std::get<std::vector<JsonObject>>(this->findValue(key)->second);
	}

	const JsonObject& JsonObject::getObject(std::string_view key) const
	{
		return std::get<JsonObject>(this->findValue(key)->second);
	}

	bool JsonObject::tryGetNull(std::string_view key) const
	{
		return this->findValue(key, false) != data.end();
	}

	bool JsonObject::tryGetString(std::string_view key, std::string& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JsonObject::tryGetBool(std::string_view key, bool& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JsonObject::tryGetInt(std::string_view key, int64_t& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jInt64_t:
			value = std::get<int64_t>(it->second);

			return true;

		case utility::VariantTypeEnum::jUInt64_t:
			value = static_cast<int64_t>(std::get<uint64_t>(it->second));

			return true;

		case utility::VariantTypeEnum::jDouble:
			value = static_cast<int64_t>(std::get<double>(it->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stoll(std::get<std::string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonObject::tryGetUnsignedInt(std::string_view key, uint64_t& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jUInt64_t:
			value = std::get<uint64_t>(it->second);

			return true;

		case utility::VariantTypeEnum::jInt64_t:
			value = static_cast<uint64_t>(std::get<int64_t>(it->second));

			return true;

		case utility::VariantTypeEnum::jDouble:
			value = static_cast<uint64_t>(std::get<double>(it->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stoull(std::get<std::string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonObject::tryGetDouble(std::string_view key, double& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jDouble:
			value = std::get<double>(it->second);

			return true;

		case utility::VariantTypeEnum::jInt64_t:
			value = static_cast<double>(std::get<int64_t>(it->second));

			return true;

		case utility::VariantTypeEnum::jUInt64_t:
			value = static_cast<double>(std::get<uint64_t>(it->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stod(std::get<std::string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonObject::tryGetArray(std::string_view key, std::vector<JsonObject>& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JsonObject::tryGetObject(std::string_view key, JsonObject& value) const
	{
		return this->tryGetValue(key, value);
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
