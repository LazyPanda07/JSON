#include "JSONUtility.h"

#include <algorithm>
#include <functional>
#include <regex>

#if defined(__LINUX__) || defined(__ANDROID__)
#include <iconv.h>
#else
#include <Windows.h>
#endif

#include "OutputOperations.h"
#include "Exceptions/WrongEncodingException.h"
#include "Exceptions/CantFindValueException.h"

namespace json::utility
{
	void JSONObject::throwCantFindValueException(std::string_view key)
	{
		throw exceptions::CantFindValueException(key);
	}

	using ConstJSONIterator = JSONObject::ConstJSONIterator;
	using ConstJSONIteratorType = JSONObject::ConstJSONIterator::ConstJSONIteratorType;

	ConstJSONIterator::ConstJSONIterator(ConstJSONIteratorType begin, ConstJSONIteratorType end, ConstJSONIteratorType start) :
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
		ConstJSONIterator it(*this);

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
		ConstJSONIterator it(*this);

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

	const std::pair<std::string, JSONObject::VariantType>& ConstJSONIterator::operator* () const noexcept
	{
		return *current;
	}

	const ConstJSONIteratorType& ConstJSONIterator::operator-> () const noexcept
	{
		return current;
	}

	bool ConstJSONIterator::operator == (const ConstJSONIterator& other) const noexcept
	{
		return current == other.current;
	}

	bool ConstJSONIterator::operator != (const ConstJSONIterator& other) const noexcept
	{
		return current != other.current;
	}

	ConstJSONIterator::operator ConstJSONIteratorType () const
	{
		return current;
	}

	template<typename T>
	JSONObject& JSONObject::setValue(std::string_view key, T&& value)
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
	bool JSONObject::tryGetValue(std::string_view key, T& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		value = std::get<T>(it->second);

		return true;
	}

	std::pair<std::vector<std::pair<std::string, JSONObject::VariantType>>::const_iterator, bool> JSONObject::find(std::string_view key, const std::vector<std::pair<std::string, VariantType>>& start, bool recursive)
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
				const std::vector<utility::JSONObject>& jsonArray = std::get<std::vector<utility::JSONObject>>(it->second);

				for (const utility::JSONObject& object : jsonArray)
				{
					auto result = JSONObject::find(key, object.data, recursive);

					if (result.second)
					{
						return result;
					}
				}
			}
			else if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
			{
				const std::vector<std::pair<std::string, VariantType>>& data = ::std::get<utility::JSONObject>(it->second).data;

				auto result = JSONObject::find(key, data, recursive);

				if (result.second)
				{
					return result;
				}
			}

			++it;
		}

		return { end, false };
	}

	ConstJSONIteratorType JSONObject::findValue(std::string_view key, bool throwException) const
	{
		auto it = find_if(data.begin(), data.end(), [key](const auto& value) { return value.first == key; });

		if (throwException && it == data.end())
		{
			throw exceptions::CantFindValueException(key);
		}

		return it;
	}

	void JSONObject::appendData(const std::string& key, const json::utility::JSONObject::VariantType& value)
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
			data.emplace_back(key, std::get<std::vector<JSONObject>>(value));

			break;

		case json::utility::VariantTypeEnum::jJSONObject:
			data.emplace_back(key, std::get<JSONObject>(value));

			break;

		default:
			break;
		}
	}

	JSONObject::JSONObject(const JSONObject& other)
	{
		(*this) = other;
	}

	JSONObject::JSONObject(JSONObject&& other) noexcept
	{
		(*this) = std::move(other);
	}

	JSONObject& JSONObject::operator = (const JSONObject& other)
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

	JSONObject& JSONObject::operator = (JSONObject&& other) noexcept
	{
		data = std::move(other.data);

		return *this;
	}

	JSONObject& JSONObject::setNull(std::string_view key)
	{
		return this->setValue(key, nullptr);
	}

	JSONObject& JSONObject::setString(std::string_view key, std::string_view value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setBool(std::string_view key, bool value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setInt(std::string_view key, int64_t value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setUnsignedInt(std::string_view key, uint64_t value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setDouble(std::string_view key, double value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setArray(std::string_view key, const std::vector<JSONObject>& value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setArray(std::string_view key, std::vector<JSONObject>&& value)
	{
		return this->setValue(key, std::move(value));
	}

	JSONObject& JSONObject::setObject(std::string_view key, const JSONObject& value)
	{
		return this->setValue(key, value);
	}

	JSONObject& JSONObject::setObject(std::string_view key, JSONObject&& value)
	{
		return this->setValue(key, std::move(value));
	}

	nullptr_t JSONObject::getNull(std::string_view key) const
	{
		return std::get<nullptr_t>(this->findValue(key)->second);
	}

	const std::string& JSONObject::getString(std::string_view key) const
	{
		return std::get<std::string>(this->findValue(key)->second);
	}

	bool JSONObject::getBool(std::string_view key) const
	{
		return std::get<bool>(this->findValue(key)->second);
	}

	int64_t JSONObject::getInt(std::string_view key) const
	{
		auto it = this->findValue(key);

		VariantTypeEnum type = static_cast<VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jUInt64_t:
			return static_cast<int64_t>(std::get<uint64_t>(it->second));

		case json::utility::VariantTypeEnum::jDouble:
			return static_cast<int64_t>(std::get<double>(it->second));
		}

		return std::get<int64_t>(it->second);
	}

	uint64_t JSONObject::getUnsignedInt(std::string_view key) const
	{
		auto it = this->findValue(key);

		VariantTypeEnum type = static_cast<VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jInt64_t:
			return static_cast<uint64_t>(std::get<int64_t>(it->second));

		case json::utility::VariantTypeEnum::jDouble:
			return static_cast<uint64_t>(std::get<double>(it->second));
		}

		return std::get<uint64_t>(it->second);
	}

	double JSONObject::getDouble(std::string_view key) const
	{
		auto it = this->findValue(key);

		VariantTypeEnum type = static_cast<VariantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::VariantTypeEnum::jInt64_t:
			return static_cast<double>(std::get<int64_t>(it->second));

		case json::utility::VariantTypeEnum::jUInt64_t:
			return static_cast<double>(std::get<uint64_t>(it->second));
		}

		return std::get<double>(it->second);
	}

	const std::vector<JSONObject>& JSONObject::getArray(std::string_view key) const
	{
		return std::get<std::vector<JSONObject>>(this->findValue(key)->second);
	}

	const JSONObject& JSONObject::getObject(std::string_view key) const
	{
		return std::get<JSONObject>(this->findValue(key)->second);
	}

	bool JSONObject::tryGetNull(std::string_view key) const
	{
		return this->findValue(key, false) != data.end();
	}

	bool JSONObject::tryGetString(std::string_view key, std::string& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JSONObject::tryGetBool(std::string_view key, bool& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JSONObject::tryGetInt(std::string_view key, int64_t& value) const
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

	bool JSONObject::tryGetUnsignedInt(std::string_view key, uint64_t& value) const
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

	bool JSONObject::tryGetDouble(std::string_view key, double& value) const
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

	bool JSONObject::tryGetArray(std::string_view key, std::vector<utility::JSONObject>& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JSONObject::tryGetObject(std::string_view key, utility::JSONObject& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool JSONObject::contains(std::string_view key, utility::VariantTypeEnum type) const
	{
		return any_of(data.begin(), data.end(), [&key, &type](const std::pair<std::string, JSONObject::VariantType>& data) { return data.first == key && data.second.index() == static_cast<size_t>(type); });
	}

	ConstJSONIterator JSONObject::begin() const noexcept
	{
		return ConstJSONIterator(data.cbegin(), data.cend(), data.cbegin());
	}

	ConstJSONIterator JSONObject::end() const noexcept
	{
		return ConstJSONIterator(data.cbegin(), data.cend(), data.cend());
	}

	JSONObject::VariantType& JSONObject::operator[](std::string_view key)
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

	const JSONObject::VariantType& JSONObject::operator[](std::string_view key) const
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

#if defined(__LINUX__) || defined(__ANDROID__)
	static std::string convertString(std::string_view source, size_t resultSize, iconv_t convert)
	{
		if (convert == reinterpret_cast<iconv_t>(std::string::npos))
		{
			throw exceptions::WrongEncodingException(source);
		}

		char* data = const_cast<char*>(source.data());
		size_t dataSize = source.size();
		std::string result(resultSize, '\0');
		char* resultData = const_cast<char*>(result.data());

		size_t error = iconv(convert, &data, &dataSize, &resultData, &resultSize);

		iconv_close(convert);

		if (error == std::string::npos)
		{
			throw exceptions::WrongEncodingException(source);
		}

		result.resize(result.size() - resultSize);

		return result;
	}

	std::string toUTF8JSON(std::string_view source, std::string_view sourceCodePage)
	{
		return convertString(source, source.size() * 4, iconv_open(CP_UTF8, sourceCodePage.data()));
	}

	std::string fromUTF8JSON(std::string_view source, std::string_view resultCodePage)
	{
		return convertString(source, source.size(), iconv_open(resultCodePage.data(), CP_UTF8));
	}
#else
	std::string toUTF8JSON(std::string_view source, uint32_t sourceCodePage)
	{
		std::string result;
		std::wstring tem;
		int size = MultiByteToWideChar
		(
			sourceCodePage,
			NULL,
			source.data(),
			-1,
			nullptr,
			NULL
		);

		if (!size)
		{
			throw exceptions::WrongEncodingException(source);
		}

		tem.resize(static_cast<size_t>(size) - 1);

		if (!MultiByteToWideChar
		(
			sourceCodePage,
			NULL,
			source.data(),
			-1,
			tem.data(),
			size
		))
		{
			throw exceptions::WrongEncodingException(source);
		}

		size = WideCharToMultiByte
		(
			CP_UTF8,
			NULL,
			tem.data(),
			-1,
			nullptr,
			NULL,
			NULL,
			NULL
		);

		if (!size)
		{
			throw exceptions::WrongEncodingException(source);
		}

		result.resize(static_cast<size_t>(size) - 1);

		if (!WideCharToMultiByte
		(
			CP_UTF8,
			NULL,
			tem.data(),
			-1,
			result.data(),
			size,
			NULL,
			NULL
		))
		{
			throw exceptions::WrongEncodingException(source);
		}

		return result;
	}

	std::string fromUTF8JSON(std::string_view source, uint32_t resultCodePage)
	{
		std::string result;
		std::wstring tem;
		int size = MultiByteToWideChar
		(
			CP_UTF8,
			NULL,
			source.data(),
			-1,
			nullptr,
			NULL
		);

		if (!size)
		{
			throw exceptions::WrongEncodingException(source);
		}

		tem.resize(static_cast<size_t>(size) - 1);

		if (!MultiByteToWideChar
		(
			CP_UTF8,
			NULL,
			source.data(),
			-1,
			tem.data(),
			size
		))
		{
			throw exceptions::WrongEncodingException(source);
		}

		size = WideCharToMultiByte
		(
			resultCodePage,
			NULL,
			tem.data(),
			-1,
			nullptr,
			NULL,
			NULL,
			NULL
		);

		if (!size)
		{
			throw exceptions::WrongEncodingException(source);
		}

		result.resize(static_cast<size_t>(size) - 1);

		if (!WideCharToMultiByte
		(
			resultCodePage,
			NULL,
			tem.data(),
			-1,
			result.data(),
			size,
			NULL,
			NULL
		))
		{
			throw exceptions::WrongEncodingException(source);
		}

		return result;
	}
#endif

	void outputJSONType(std::ostream& outputStream, const JSONObject::VariantType& value, bool isLast, std::string& offset)
	{
		VariantTypeEnum type = static_cast<VariantTypeEnum>(value.index());

		if (type >= VariantTypeEnum::jJSONArray)
		{
			offset += "  ";
		}

		switch (type)
		{
		case VariantTypeEnum::jNull:
			outputStream << "null";

			break;

		case VariantTypeEnum::jString:
			outputStream << '"' << std::regex_replace(std::get<std::string>(value), std::regex(R"(\\)"), R"(\\)") << '"';

			break;

		case VariantTypeEnum::jBool:
			outputStream << std::boolalpha << std::get<bool>(value);

			break;

		case VariantTypeEnum::jInt64_t:
			outputStream << std::get<int64_t>(value);

			break;

		case VariantTypeEnum::jUInt64_t:
			outputStream << std::get<uint64_t>(value);

			break;

		case VariantTypeEnum::jDouble:
			outputStream << std::fixed << std::get<double>(value);

			break;

		case VariantTypeEnum::jJSONArray:
			outputStream << JSONArrayWrapper(value, &offset) << std::string(offset.begin(), offset.end() - 2) << ']';

			break;

		case VariantTypeEnum::jJSONObject:
		{
			const JSONObject& ref = std::get<JSONObject>(value);

			auto start = ref.data.begin();
			auto end = ref.data.end();

			outputStream << "{\n";

			while (start != end)
			{
				auto check = start;

				if (start->first.size())
				{
					outputStream << offset << '"' << start->first << '"' << ": ";
				}
				else
				{
					outputStream << offset;
				}

				outputJSONType(outputStream, start->second, ++check == end, offset);

				++start;
			}

			outputStream << std::string(offset.begin(), offset.end() - 2) << '}';
		}

		break;
		}

		if (type >= VariantTypeEnum::jJSONArray)
		{
			offset.pop_back();
			offset.pop_back();
		}

		if (!isLast)
		{
			outputStream << ',';
		}

		outputStream << std::endl;
	}

	void appendArray(JSONObject::VariantType&& value, std::vector<JSONObject>& jsonArray)
	{
		JSONObject object;

		object.data.emplace_back("", std::move(value));

		jsonArray.push_back(std::move(object));
	}

	std::string getJSONVersion()
	{
		std::string jsonVersion = "2.8.0";

		return jsonVersion;
	}
}
