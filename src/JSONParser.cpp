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
#pragma warning(push)
#pragma warning(disable: 4018)
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

#pragma warning(pop)
	}

	void JSONParser::insertKeyValueData(string&& key, const string& value, utility::jsonObject& ptr)
	{
		ptr.data.push_back({ move(key), JSONParser::parseValue(value) });
	}

	pair<vector<pair<string, JSONParser::variantType>>::const_iterator, bool> JSONParser::find(const string& key, const vector<pair<string, variantType>>& start, bool recursive)
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
				const vector<utility::jsonObject>& jsonArray = get<static_cast<size_t>(utility::variantTypeEnum::jJSONArray)>(it->second);

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
				const vector<pair<string, variantType>>& data = ::get<utility::jsonObject>(it->second).data;

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

	template<utility::variantTypeEnum ResultType>
	bool JSONParser::checkDifferType(const variantType& checkType)
	{
		return ResultType != static_cast<utility::variantTypeEnum>(checkType.index());
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

	JSONParser::JSONParser(const string& data) :
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

	bool JSONParser::contains(const string& key, utility::variantTypeEnum type, bool recursive) const
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
					const auto& object = get<static_cast<size_t>(utility::variantTypeEnum::jJSONObject)>(currentValue);

					objects.push(&object);
				}
			}
		}

		return false;
	}

#ifdef __LINUX__
	void JSONParser::setJSONData(const string& jsonData, string_view codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#else
	void JSONParser::setJSONData(const string& jsonData, uint32_t codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#endif

	void JSONParser::setJSONData(const string& jsonData)
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

	void JSONParser::setJSONData(string&& jsonData) noexcept
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

	template<>
	JSON_API const nullptr_t& JSONParser::getValue<nullptr_t>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<nullptr_t>(result->second);
	}

	template<>
	JSON_API const string& JSONParser::getValue<string>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<string>(result->second);
	}

	template<>
	JSON_API const bool& JSONParser::getValue<bool>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<bool>(result->second);
	}

	template<>
	JSON_API const int64_t& JSONParser::getValue<int64_t>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<int64_t>(result->second);
	}

	template<>
	JSON_API const uint64_t& JSONParser::getValue<uint64_t>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<uint64_t>(result->second);
	}

	template<>
	JSON_API const double& JSONParser::getValue<double>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<double>(result->second);
	}

	template<>
	JSON_API const vector<utility::jsonObject>& JSONParser::getValue<vector<utility::jsonObject>>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<vector<utility::jsonObject>>(result->second);
	}

	template<>
	JSON_API const utility::jsonObject& JSONParser::getValue<utility::jsonObject>(const string& key, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return get<utility::jsonObject>(result->second);
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<nullptr_t>(const string& key, nullptr_t& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<nullptr_t>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<string>(const string& key, string& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success || JSONParser::checkDifferType<json::utility::variantTypeEnum::jString>(result->second))
		{
			return false;
		}

		value = get<string>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<bool>(const string& key, bool& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<bool>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<int64_t>(const string& key, int64_t& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<int64_t>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<uint64_t>(const string& key, uint64_t& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<uint64_t>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<double>(const string& key, double& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<double>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<vector<utility::jsonObject>>(const string& key, vector<utility::jsonObject>& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<vector<utility::jsonObject>>(result->second);

		return true;
	}

	template<>
	JSON_API bool JSONParser::tryGetValue<utility::jsonObject>(const string& key, utility::jsonObject& value, bool recursive) const
	{
		auto [result, success] = JSONParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		value = get<utility::jsonObject>(result->second);

		return true;
	}

	nullptr_t JSONParser::getNull(const string& key, bool recursive) const
	{
		return this->getValue<nullptr_t>(key, recursive);
	}

	const string& JSONParser::getString(const string& key, bool recursive) const
	{
		return this->getValue<string>(key, recursive);
	}

	bool JSONParser::getBool(const string& key, bool recursive) const
	{
		return this->getValue<bool>(key, recursive);
	}

	int64_t JSONParser::getInt(const string& key, bool recursive) const
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
			return static_cast<int64_t>(get<uint64_t>(result->second));

		case utility::variantTypeEnum::jDouble:
			return static_cast<int64_t>(get<double>(result->second));

		case utility::variantTypeEnum::jString:
			return stoll(get<string>(result->second));
		}

		return get<int64_t>(result->second);
	}

	uint64_t JSONParser::getUnsignedInt(const string& key, bool recursive) const
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
			return static_cast<uint64_t>(get<int64_t>(result->second));

		case utility::variantTypeEnum::jDouble:
			return static_cast<uint64_t>(get<double>(result->second));

		case utility::variantTypeEnum::jString:
			return stoull(get<string>(result->second));
		}

		return get<uint64_t>(result->second);
	}

	double JSONParser::getDouble(const string& key, bool recursive) const
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
			return static_cast<double>(get<int64_t>(result->second));

		case utility::variantTypeEnum::jUInt64_t:
			return static_cast<double>(get<uint64_t>(result->second));

		case utility::variantTypeEnum::jString:
			return stod(get<string>(result->second));
		}

		return get<double>(result->second);
	}

	const vector<utility::jsonObject>& JSONParser::getArray(const string& key, bool recursive) const
	{
		return this->getValue<vector<utility::jsonObject>>(key);
	}

	const utility::jsonObject& JSONParser::getObject(const string& key, bool recursive) const
	{
		return this->getValue<utility::jsonObject>(key);
	}

	bool JSONParser::tryGetNull(const string& key, bool recursive) const
	{
		nullptr_t value;

		return this->tryGetValue(key, value, recursive);
	}

	bool JSONParser::tryGetString(const string& key, string& value, bool recursive) const
	{
		return this->tryGetValue(key, value, recursive);
	}

	bool JSONParser::tryGetBool(const string& key, bool& value, bool recursive) const
	{
		return this->tryGetValue(key, value, recursive);
	}

	bool JSONParser::tryGetInt(const string& key, int64_t& value, bool recursive) const
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
			value = get<int64_t>(result->second);

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<int64_t>(get<uint64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<int64_t>(get<double>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoll(get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetUnsignedInt(const string& key, uint64_t& value, bool recursive) const
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
			value = get<uint64_t>(result->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<uint64_t>(get<int64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jDouble:
			value = static_cast<uint64_t>(get<double>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stoull(get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetDouble(const string& key, double& value, bool recursive) const
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
			value = get<double>(result->second);

			return true;

		case utility::variantTypeEnum::jInt64_t:
			value = static_cast<double>(get<int64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jUInt64_t:
			value = static_cast<double>(get<uint64_t>(result->second));

			return true;

		case utility::variantTypeEnum::jString:
			value = stod(get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JSONParser::tryGetArray(const string& key, vector<utility::jsonObject>& value, bool recursive) const
	{
		return this->tryGetValue(key, value, recursive);
	}

	bool JSONParser::tryGetObject(const string& key, utility::jsonObject& value, bool recursive) const
	{
		return this->tryGetValue(key, value, recursive);
	}

	const utility::jsonObject& JSONParser::getParsedData() const
	{
		return parsedData;
	}

	void JSONParser::getParsedData(utility::jsonObject& object) noexcept
	{
		object = move(parsedData);
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
