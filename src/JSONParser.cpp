#include "JSONParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>
#include <queue>
#include <limits>

#pragma warning(disable: 4715)
#pragma warning(disable: 26800)

using namespace std;

constexpr char openCurlyBracket = '{';
constexpr char closeCurlyBracket = '}';
constexpr char openSquareBracket = '[';
constexpr char closeSquareBracket = ']';
constexpr char comma = ',';
constexpr char colon = ':';

bool isNumber(const string& source);

namespace json
{
	using ConstJSONIterator = utility::jsonObject::ConstJSONIterator;

	utility::jsonObject::variantType JSONParser::parseValue(const string& value)
	{
#ifndef __LINUX__
#pragma warning(push)
#pragma warning(disable: 4018)
#endif

		if (isStringSymbol(*value.begin()) && isStringSymbol(*value.rbegin()))
		{
			return string(value.begin() + 1, value.end() - 1);
		}
		else if (value == "true" || value == "false")
		{
			return value == "true";
		}
		else if (value == "null")
		{
			return nullptr;
		}
		else if (isNumber(value))
		{
			if (value.find('.') != string::npos)
			{
				return stod(value);
			}
			else
			{
				if (value.find('-') != string::npos)
				{
					return stoll(value);
				}
				else if (uint64_t valueToInsert = stoull(value); valueToInsert > numeric_limits<int64_t>::max())
				{
					return valueToInsert;
				}
				else
				{
					return stoll(value);
				}
			}
		}

		throw runtime_error("Can't parse value");

		return nullptr;
#ifndef __LINUX__
#pragma warning(pop)
#endif
	}

	void JSONParser::insertKeyValueData(string&& key, const string& value, utility::jsonObject& ptr)
	{
		ptr.data.emplace_back(move(key), JSONParser::parseValue(value));
	}

	pair<vector<pair<string, JSONParser::variantType>>::const_iterator, bool> JSONParser::find(string_view key, const vector<pair<string, variantType>>& start, bool recursive)
	{
		auto it = find_if(start.begin(), start.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });
		auto end = start.end();

		if (!recursive || it != end)
		{
			return { it, it != end };
		}

		it = start.begin();

		while (it != end)
		{
			if (it->second.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONArray))
			{
				const vector<utility::jsonObject>& jsonArray = std::get<vector<utility::jsonObject>>(it->second);

				for (const utility::jsonObject& object : jsonArray)
				{
					auto result = JSONParser::find(key, object.data, recursive);

					if (result.second)
					{
						return result;
					}
				}
			}
			else if (it->second.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONObject))
			{
				const vector<pair<string, variantType>>& data = ::std::get<utility::jsonObject>(it->second).data;

				auto result = JSONParser::find(key, data, recursive);

				if (result.second)
				{
					return result;
				}
			}

			++it;
		}

		return { end, false };
	}

	bool JSONParser::isStringSymbol(char symbol)
	{
		return symbol == '"';
	}

	char JSONParser::interpretEscapeSymbol(char symbol)
	{
		switch (symbol)
		{
		case 'n':
			return '\n';

		case 't':
			return '\t';

		case '"':
			return '"';

		case '\\':
			return '\\';

		case 'r':
			return '\r';

		case 'b':
			return '\b';

		case 'f':
			return '\f';

		default:
			return symbol;
		}
	}

	void JSONParser::parse()
	{
		using namespace json::utility;

		enum class type
		{
			object,
			array
		};

		stack<pair<string, jsonObject*>> objects;
		stack<pair<string, vector<jsonObject>>> arrays;
		stack<type> currentTop;
		string key;
		string value;
		bool startString = false;
		bool escapeSymbol = false;
		size_t index = 0;

		if (rawData.size() && rawData[0] == '{')
		{
			objects.push({ "", &parsedData });

			currentTop.push(type::object);

			index = 1;
		}
		else if (rawData.size() && rawData[0] == '[')
		{
			objects.push({ "", &parsedData });

			currentTop.push(type::object);
		}

		for (; index < rawData.size(); index++)
		{
			char c = rawData[index];

			if (!escapeSymbol && c == '\\')
			{
				escapeSymbol = true;

				continue;
			}
			else if (escapeSymbol)
			{
				escapeSymbol = false;

				value += JSONParser::interpretEscapeSymbol(c);

				continue;
			}

			if (!startString && isStringSymbol(c))
			{
				startString = true;
			}
			else if (startString && isStringSymbol(c))
			{
				startString = false;
			}

			if (startString)
			{
				value += c;

				continue;
			}

			if (isspace(static_cast<unsigned char>(c)))
			{
				continue;
			}

			switch (c)
			{
			case openCurlyBracket:
				objects.push({ move(key), new jsonObject() });

				currentTop.push(type::object);

				break;

			case closeCurlyBracket:
				if (value.size())
				{
					JSONParser::insertKeyValueData(move(key), value, *objects.top().second);

					value.clear();
				}

				{
					pair<string, jsonObject*> object = move(objects.top());
					vector<pair<string, jsonObject::variantType>>* data = nullptr;

					objects.pop();
					currentTop.pop();

					if (object.second == &parsedData)
					{
						continue;
					}

					switch (currentTop.top())
					{
					case type::object:
						data = &objects.top().second->data;

						break;

					case type::array:
						data = &arrays.top().second.emplace_back().data;

						break;

					default:
						throw runtime_error("Wrong type");
					}

					data->push_back({ move(object.first), jsonObject(*object.second) });

					delete object.second;
				}

				break;

			case openSquareBracket:
				currentTop.push(type::array);

				arrays.push({ move(key), vector<jsonObject>() });

				break;

			case closeSquareBracket:
				if (value.size())
				{
					JSONParser::insertKeyValueData(move(key), value, arrays.top().second.emplace_back());

					value.clear();
				}

				{
					pair<string, vector<jsonObject>> array = arrays.top();
					vector<pair<string, jsonObject::variantType>>* data = nullptr;

					arrays.pop();
					currentTop.pop();

					switch (currentTop.top())
					{
					case type::object:
						data = &objects.top().second->data;

						break;

					case type::array:
						data = &arrays.top().second.emplace_back().data;

						break;

					default:
						throw runtime_error("Wrong type");
					}

					data->push_back(move(array));
				}

				break;

			case comma:
				if (isNumber(value) || (value.size() && isStringSymbol(*value.begin()) && isStringSymbol(*value.rbegin())) || (value == "true" || value == "false" || value == "null"))
				{
					JSONParser::insertKeyValueData
					(
						move(key),
						value,
						currentTop.top() == type::array ? arrays.top().second.emplace_back() : *objects.top().second
					);

					value.clear();
				}
				else if (value.size())
				{
					value += c;
				}

				break;

			case colon:
				key = string(value.begin() + 1, value.end() - 1);

				value.clear();

				break;

			default:
				value += c;
			}
		}
	}

	JSONParser::JSONParser(string_view data) :
		rawData(utility::toUTF8JSON(data, CP_UTF8))
	{
		this->parse();
	}

	JSONParser::JSONParser(istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	JSONParser::JSONParser(istream&& inputStream) :
		JSONParser(inputStream)
	{

	}

	JSONParser::JSONParser(const utility::jsonObject& data) :
		parsedData(data)
	{

	}

	JSONParser::JSONParser(const JSONParser& other) :
		rawData(other.rawData),
		parsedData(other.parsedData)
	{

	}

	JSONParser::JSONParser(JSONParser&& other) noexcept :
		rawData(move(other.rawData)),
		parsedData(move(other.parsedData))
	{

	}

	JSONParser& JSONParser::operator = (const JSONParser& other)
	{
		rawData = other.rawData;
		parsedData = other.parsedData;

		return *this;
	}

	JSONParser& JSONParser::operator = (JSONParser&& other) noexcept
	{
		rawData = move(other.rawData);
		parsedData = move(other.parsedData);

		return *this;
	}

	bool JSONParser::contains(string_view key, utility::variantTypeEnum type, bool recursive) const
	{
		queue<const utility::jsonObject*> objects;

		objects.push(&parsedData);

		while (objects.size())
		{
			const utility::jsonObject* current = objects.front();

			objects.pop();

			for (const auto& [currentKey, currentValue] : current->data)
			{
				if (currentKey == key && currentValue.index() == static_cast<size_t>(type))
				{
					return true;
				}

				if (recursive && currentValue.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONObject))
				{
					const utility::jsonObject& object = std::get<utility::jsonObject>(currentValue);

					objects.push(&object);
				}
			}
		}

		return false;
	}

#if defined(__LINUX__) || defined(__ANDROID__)
	void JSONParser::setJSONData(string_view jsonData, string_view codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#else
	void JSONParser::setJSONData(string_view jsonData, uint32_t codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#endif

	void JSONParser::setJSONData(string_view jsonData)
	{
		rawData = jsonData;

		this->parse();
	}

	void JSONParser::setJSONData(istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JSONParser::setJSONData(istream&& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JSONParser::setJSONData(string&& jsonData)
	{
		rawData = move(jsonData);

		this->parse();
	}

	ConstJSONIterator JSONParser::begin() const noexcept
	{
		return parsedData.begin();
	}

	ConstJSONIterator JSONParser::end() const noexcept
	{
		return parsedData.end();
	}

	const string& JSONParser::getRawData() const
	{
		return rawData;
	}

	const string& JSONParser::operator * () const
	{
		return rawData;
	}

	nullptr_t JSONParser::getNull(string_view key, bool recursive) const
	{
		return this->get<nullptr_t>(key, recursive);
	}

	const string& JSONParser::getString(string_view key, bool recursive) const
	{
		return this->get<string>(key, recursive);
	}

	bool JSONParser::getBool(string_view key, bool recursive) const
	{
		return this->get<bool>(key, recursive);
	}

	int64_t JSONParser::getInt(string_view key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jUInt64_t:
			return static_cast<int64_t>(std::get<uint64_t>(result->second));

		case utility::variantTypeEnum::jDouble:
			return static_cast<int64_t>(std::get<double>(result->second));

		case utility::variantTypeEnum::jString:
			return stoll(std::get<string>(result->second));
		}

		return std::get<int64_t>(result->second);
	}

	uint64_t JSONParser::getUnsignedInt(string_view key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jInt64_t:
			return static_cast<uint64_t>(std::get<int64_t>(result->second));

		case utility::variantTypeEnum::jDouble:
			return static_cast<uint64_t>(std::get<double>(result->second));

		case utility::variantTypeEnum::jString:
			return stoull(std::get<string>(result->second));
		}

		return std::get<uint64_t>(result->second);
	}

	double JSONParser::getDouble(string_view key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jInt64_t:
			return static_cast<double>(std::get<int64_t>(result->second));

		case utility::variantTypeEnum::jUInt64_t:
			return static_cast<double>(std::get<uint64_t>(result->second));

		case utility::variantTypeEnum::jString:
			return stod(std::get<string>(result->second));
		}

		return std::get<double>(result->second);
	}

	const vector<utility::jsonObject>& JSONParser::getArray(string_view key, bool recursive) const
	{
		return this->get<vector<utility::jsonObject>>(key);
	}

	const utility::jsonObject& JSONParser::getObject(string_view key, bool recursive) const
	{
		return this->get<utility::jsonObject>(key);
	}

	bool JSONParser::tryGetNull(string_view key, bool recursive) const
	{
		nullptr_t value;

		return this->tryGet(key, value, recursive);
	}

	bool JSONParser::tryGetString(string_view key, string& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JSONParser::tryGetBool(string_view key, bool& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JSONParser::tryGetInt(string_view key, int64_t& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jInt64_t:
			value = std::get<int64_t>(result->second);

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<int64_t>(std::get<uint64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<int64_t>(std::get<double>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoll(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetUnsignedInt(string_view key, uint64_t& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jUInt64_t:
			value = std::get<uint64_t>(result->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<uint64_t>(std::get<int64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<uint64_t>(std::get<double>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoull(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetDouble(string_view key, double& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::variantTypeEnum type = static_cast<utility::variantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::variantTypeEnum::jDouble:
			value = std::get<double>(result->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<double>(std::get<int64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<double>(std::get<uint64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stod(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetArray(string_view key, vector<utility::jsonObject>& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JSONParser::tryGetObject(string_view key, utility::jsonObject& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	const utility::jsonObject& JSONParser::getParsedData() const
	{
		return parsedData;
	}

	void JSONParser::getParsedData(utility::jsonObject& object) noexcept
	{
		object = move(parsedData);
	}

	void JSONParser::overrideValue(string_view key, const variantType& value, bool recursive)
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		const_cast<variantType&>(result->second) = value;
	}

	void JSONParser::overrideValue(string_view key, variantType&& value, bool recursive)
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		const_cast<variantType&>(result->second) = move(value);
	}

	JSON_API istream& operator >> (istream& inputStream, JSONParser& parser)
	{
		parser.setJSONData(inputStream);

		return inputStream;
	}

	JSON_API ostream& operator << (ostream& outputStream, const JSONParser& parser)
	{
		ConstJSONIterator start = parser.begin();
		ConstJSONIterator end = parser.end();
		string offset = "  ";

		outputStream << '{' << endl;

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

			utility::outputJSONType(outputStream, start->second, ++check == end, offset);

			++start;
		}

		outputStream << '}';

		return outputStream;
	}
}

bool isNumber(const string& source)
{
	if (source.empty())
	{
		return false;
	}

	static constexpr string_view symbols = "0123456789-.";

	if (all_of(source.begin(), source.end(), [](const char& c) { return find(symbols.begin(), symbols.end(), c) != symbols.end(); }))
	{
		size_t check = 0;

		for (const auto& c : source)
		{
			if (c == '.')
			{
				check++;
			}
		}

		if (check > 1)
		{
			return false;
		}

		return true;
	}

	return false;
}
