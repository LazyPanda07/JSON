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

using namespace std;

namespace json::utility
{
	using ConstJSONIterator = jsonObject::ConstJSONIterator;
	using ConstJSONIteratorType = jsonObject::ConstJSONIterator::ConstJSONIteratorType;

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

	const pair<string, jsonObject::variantType>& ConstJSONIterator::operator* () const noexcept
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
	jsonObject& jsonObject::setValue(string_view key, T&& value)
	{
		if constexpr (is_same_v<string_view&, decltype(value)>)
		{
			data.emplace_back(string(key.data(), key.size()), string(value.data(), value.size()));
		}
		else
		{
			data.emplace_back(string(key.data(), key.size()), forward<T>(value));
		}

		return *this;
	}

	template<typename T>
	bool jsonObject::tryGetValue(string_view key, T& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		value = get<T>(it->second);

		return true;
	}

	ConstJSONIteratorType jsonObject::findValue(string_view key, bool throwException) const
	{
		auto it = find_if(data.begin(), data.end(), [key](const auto& value) { return value.first == key; });

		if (throwException && it == data.end())
		{
			throw exceptions::CantFindValueException(key);
		}

		return it;
	}

	void jsonObject::appendData(const string& key, const json::utility::jsonObject::variantType& value)
	{
		switch (static_cast<json::utility::variantTypeEnum>(value.index()))
		{
		case json::utility::variantTypeEnum::jNull:
			data.emplace_back(key, get<nullptr_t>(value));

			break;

		case json::utility::variantTypeEnum::jString:
			data.emplace_back(key, get<string>(value));

			break;

		case json::utility::variantTypeEnum::jBool:
			data.emplace_back(key, get<bool>(value));

			break;

		case json::utility::variantTypeEnum::jInt64_t:
			data.emplace_back(key, get<int64_t>(value));

			break;

		case json::utility::variantTypeEnum::jUInt64_t:
			data.emplace_back(key, get<uint64_t>(value));

			break;

		case json::utility::variantTypeEnum::jDouble:
			data.emplace_back(key, get<double>(value));

			break;

		case json::utility::variantTypeEnum::jJSONArray:
			data.emplace_back(key, get<vector<jsonObject>>(value));

			break;

		case json::utility::variantTypeEnum::jJSONObject:
			data.emplace_back(key, get<jsonObject>(value));

			break;

		default:
			break;
		}
	}

	jsonObject::jsonObject(const jsonObject& other)
	{
		(*this) = other;
	}

	jsonObject::jsonObject(jsonObject&& other) noexcept
	{
		(*this) = move(other);
	}

	jsonObject& jsonObject::operator = (const jsonObject& other)
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

	jsonObject& jsonObject::operator = (jsonObject&& other) noexcept
	{
		data = move(other.data);

		return *this;
	}

	jsonObject& jsonObject::setNull(string_view key)
	{
		return this->setValue(key, nullptr);
	}

	jsonObject& jsonObject::setString(string_view key, string_view value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setBool(string_view key, bool value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setInt(string_view key, int64_t value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setUnsignedInt(string_view key, uint64_t value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setDouble(string_view key, double value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setArray(string_view key, const vector<jsonObject>& value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setArray(string_view key, vector<jsonObject>&& value)
	{
		return this->setValue(key, move(value));
	}

	jsonObject& jsonObject::setObject(string_view key, const jsonObject& value)
	{
		return this->setValue(key, value);
	}

	jsonObject& jsonObject::setObject(string_view key, jsonObject&& value)
	{
		return this->setValue(key, move(value));
	}

	nullptr_t jsonObject::getNull(string_view key) const
	{
		return get<nullptr_t>(this->findValue(key)->second);
	}

	const string& jsonObject::getString(string_view key) const
	{
		return get<string>(this->findValue(key)->second);
	}

	bool jsonObject::getBool(string_view key) const
	{
		return get<bool>(this->findValue(key)->second);
	}

	int64_t jsonObject::getInt(string_view key) const
	{
		auto it = this->findValue(key);

		variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::variantTypeEnum::jUInt64_t:
			return static_cast<int64_t>(get<uint64_t>(it->second));

		case json::utility::variantTypeEnum::jDouble:
			return static_cast<int64_t>(get<double>(it->second));
		}

		return get<int64_t>(it->second);
	}

	uint64_t jsonObject::getUnsignedInt(string_view key) const
	{
		auto it = this->findValue(key);

		variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::variantTypeEnum::jInt64_t:
			return static_cast<uint64_t>(get<int64_t>(it->second));

		case json::utility::variantTypeEnum::jDouble:
			return static_cast<uint64_t>(get<double>(it->second));
		}

		return get<uint64_t>(it->second);
	}

	double jsonObject::getDouble(string_view key) const
	{
		auto it = this->findValue(key);

		variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

		switch (type)
		{
		case json::utility::variantTypeEnum::jInt64_t:
			return static_cast<double>(get<int64_t>(it->second));

		case json::utility::variantTypeEnum::jUInt64_t:
			return static_cast<double>(get<uint64_t>(it->second));
		}

		return get<double>(it->second);
	}

	const vector<jsonObject>& jsonObject::getArray(string_view key) const
	{
		return get<vector<jsonObject>>(this->findValue(key)->second);
	}

	const jsonObject& jsonObject::getObject(string_view key) const
	{
		return get<jsonObject>(this->findValue(key)->second);
	}

	bool jsonObject::tryGetNull(string_view key) const
	{
		return this->findValue(key, false) != data.end();
	}

	bool jsonObject::tryGetString(string_view key, string& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool jsonObject::tryGetBool(string_view key, bool& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool jsonObject::tryGetInt(string_view key, int64_t& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jInt64_t:
			value = get<int64_t>(it->second);

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<int64_t>(get<uint64_t>(it->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<int64_t>(get<double>(it->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoll(get<string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool jsonObject::tryGetUnsignedInt(string_view key, uint64_t& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jUInt64_t:
			value = get<uint64_t>(it->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<uint64_t>(get<int64_t>(it->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<uint64_t>(get<double>(it->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoull(get<string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool jsonObject::tryGetDouble(string_view key, double& value) const
	{
		auto it = this->findValue(key, false);

		if (it == data.end())
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(it->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jDouble:
			value = get<double>(it->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<double>(get<int64_t>(it->second));

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<double>(get<uint64_t>(it->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stod(get<string>(it->second));

			return true;

		default:
			return false;
		}
	}

	bool jsonObject::tryGetArray(string_view key, vector<utility::jsonObject>& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool jsonObject::tryGetObject(string_view key, utility::jsonObject& value) const
	{
		return this->tryGetValue(key, value);
	}

	bool jsonObject::contains(string_view key, utility::variantTypeEnum type) const
	{
		return any_of(data.begin(), data.end(), [&key, &type](const pair<string, jsonObject::variantType>& data) { return data.first == key && data.second.index() == static_cast<size_t>(type); });
	}

	ConstJSONIterator jsonObject::begin() const noexcept
	{
		return ConstJSONIterator(data.cbegin(), data.cend(), data.cbegin());
	}

	ConstJSONIterator jsonObject::end() const noexcept
	{
		return ConstJSONIterator(data.cbegin(), data.cend(), data.cend());
	}

	jsonObject::variantType& jsonObject::operator[](string_view key)
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

	const jsonObject::variantType& jsonObject::operator[](string_view key) const
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
	static string convertString(string_view source, size_t resultSize, iconv_t convert)
	{
		if (convert == reinterpret_cast<iconv_t>(string::npos))
		{
			throw exceptions::WrongEncodingException(source);
		}

		char* data = const_cast<char*>(source.data());
		size_t dataSize = source.size();
		string result(resultSize, '\0');
		char* resultData = const_cast<char*>(result.data());

		size_t error = iconv(convert, &data, &dataSize, &resultData, &resultSize);

		iconv_close(convert);

		if (error == string::npos)
		{
			throw exceptions::WrongEncodingException(source);
		}

		result.resize(result.size() - resultSize);

		return result;
	}

	string toUTF8JSON(string_view source, string_view sourceCodePage)
	{
		return convertString(source, source.size() * 4, iconv_open(CP_UTF8, sourceCodePage.data()));
	}

	string fromUTF8JSON(string_view source, string_view resultCodePage)
	{
		return convertString(source, source.size(), iconv_open(resultCodePage.data(), CP_UTF8));
	}
#else
	string toUTF8JSON(string_view source, uint32_t sourceCodePage)
	{
		string result;
		wstring tem;
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

	string fromUTF8JSON(string_view source, uint32_t resultCodePage)
	{
		string result;
		wstring tem;
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

	void outputJSONType(ostream& outputStream, const jsonObject::variantType& value, bool isLast, string& offset)
	{
		variantTypeEnum type = static_cast<variantTypeEnum>(value.index());

		if (type >= variantTypeEnum::jJSONArray)
		{
			offset += "  ";
		}

		switch (type)
		{
		case variantTypeEnum::jNull:
			outputStream << "null";

			break;

		case variantTypeEnum::jString:
			outputStream << '"' << regex_replace(get<string>(value), regex(R"(\\)"), R"(\\)") << '"';

			break;

		case variantTypeEnum::jBool:
			outputStream << boolalpha << get<bool>(value);

			break;

		case variantTypeEnum::jInt64_t:
			outputStream << get<int64_t>(value);

			break;

		case variantTypeEnum::jUInt64_t:
			outputStream << get<uint64_t>(value);

			break;

		case variantTypeEnum::jDouble:
			outputStream << fixed << get<double>(value);

			break;

		case variantTypeEnum::jJSONArray:
			outputStream << JSONArrayWrapper(value, &offset) << string(offset.begin(), offset.end() - 2) << ']';

			break;

		case variantTypeEnum::jJSONObject:
		{
			const jsonObject& ref = get<jsonObject>(value);

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

			outputStream << string(offset.begin(), offset.end() - 2) << '}';
		}

		break;
		}

		if (type >= variantTypeEnum::jJSONArray)
		{
			offset.pop_back();
			offset.pop_back();
		}

		if (!isLast)
		{
			outputStream << ',';
		}

		outputStream << endl;
	}

	void appendArray(jsonObject::variantType&& value, vector<jsonObject>& jsonArray)
	{
		jsonObject object;

		object.data.emplace_back("", move(value));

		jsonArray.push_back(move(object));
	}

	string getJSONVersion()
	{
		string jsonVersion = "2.7.1";

		return jsonVersion;
	}
}
