#include "JsonParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>
#include <queue>
#include <limits>
#include <regex>

#include "JsonArrayWrapper.h"
#include "OutputOperations.h"

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
	using ConstJSONIterator = JsonObject::ConstIterator;

	JsonObject::VariantType JsonParser::parseValue(const string& value)
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

	void JsonParser::insertKeyValueData(string&& key, const string& value, JsonObject& ptr)
	{
		ptr.data.emplace_back(move(key), JsonParser::parseValue(value));
	}

	pair<vector<pair<string, JsonParser::VariantType>>::const_iterator, bool> JsonParser::find(string_view key, const vector<pair<string, VariantType>>& start, bool recursive)
	{
		auto it = find_if(start.begin(), start.end(), [&key](const pair<string, VariantType>& value) { return value.first == key; });
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
				const vector<JsonObject>& jsonArray = std::get<vector<JsonObject>>(it->second);

				for (const JsonObject& object : jsonArray)
				{
					auto result = JsonParser::find(key, object.data, recursive);

					if (result.second)
					{
						return result;
					}
				}
			}
			else if (it->second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
			{
				const vector<pair<string, VariantType>>& data = ::std::get<JsonObject>(it->second).data;

				auto result = JsonParser::find(key, data, recursive);

				if (result.second)
				{
					return result;
				}
			}

			++it;
		}

		return { end, false };
	}

	bool JsonParser::isStringSymbol(char symbol)
	{
		return symbol == '"';
	}

	char JsonParser::interpretEscapeSymbol(char symbol)
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

	void JsonParser::parse()
	{
		using namespace json::utility;

		enum class type
		{
			object,
			array
		};

		static const std::regex singleLine(R"(//[^\n\r]*)", std::regex_constants::ECMAScript);
		static const std::regex multiLine(R"(/\*([^*]|\*+[^*/])*\*+/)", std::regex_constants::ECMAScript);

		rawData = std::regex_replace(rawData, singleLine, ""); // Remove // comments
		rawData = std::regex_replace(rawData, multiLine, ""); // Remove /* ... */ comments (including multiline)

		stack<pair<string, JsonObject*>> objects;
		stack<pair<string, vector<JsonObject>>> arrays;
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

				value += JsonParser::interpretEscapeSymbol(c);

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
				objects.push({ move(key), new JsonObject() });

				currentTop.push(type::object);

				break;

			case closeCurlyBracket:
				if (value.size())
				{
					JsonParser::insertKeyValueData(move(key), value, *objects.top().second);

					value.clear();
				}

				{
					pair<string, JsonObject*> object = move(objects.top());
					vector<pair<string, JsonObject::VariantType>>* data = nullptr;

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

					data->push_back({ move(object.first), JsonObject(*object.second) });

					delete object.second;
				}

				break;

			case openSquareBracket:
				currentTop.push(type::array);

				arrays.push({ move(key), vector<JsonObject>() });

				break;

			case closeSquareBracket:
				if (value.size())
				{
					JsonParser::insertKeyValueData(move(key), value, arrays.top().second.emplace_back());

					value.clear();
				}

				{
					pair<string, vector<JsonObject>> array = arrays.top();
					vector<pair<string, JsonObject::VariantType>>* data = nullptr;

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
					JsonParser::insertKeyValueData
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

	JsonParser::JsonParser(string_view data) :
		rawData(utility::toUTF8JSON(data, CP_UTF8))
	{
		this->parse();
	}

	JsonParser::JsonParser(istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	JsonParser::JsonParser(istream&& inputStream) :
		JsonParser(inputStream)
	{

	}

	JsonParser::JsonParser(const JsonObject& data) :
		parsedData(data)
	{

	}

	JsonParser::JsonParser(const JsonParser& other) :
		rawData(other.rawData),
		parsedData(other.parsedData)
	{

	}

	JsonParser::JsonParser(JsonParser&& other) noexcept :
		rawData(move(other.rawData)),
		parsedData(move(other.parsedData))
	{

	}

	JsonParser& JsonParser::operator = (const JsonParser& other)
	{
		rawData = other.rawData;
		parsedData = other.parsedData;

		return *this;
	}

	JsonParser& JsonParser::operator = (JsonParser&& other) noexcept
	{
		rawData = move(other.rawData);
		parsedData = move(other.parsedData);

		return *this;
	}

	bool JsonParser::contains(string_view key, utility::VariantTypeEnum type, bool recursive) const
	{
		queue<const JsonObject*> objects;

		objects.push(&parsedData);

		while (objects.size())
		{
			const JsonObject* current = objects.front();

			objects.pop();

			for (const auto& [currentKey, currentValue] : current->data)
			{
				if (currentKey == key && currentValue.index() == static_cast<size_t>(type))
				{
					return true;
				}

				if (recursive && currentValue.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
				{
					const JsonObject& object = std::get<JsonObject>(currentValue);

					objects.push(&object);
				}
			}
		}

		return false;
	}

#if defined(__LINUX__) || defined(__ANDROID__)
	void JsonParser::setJSONData(string_view jsonData, string_view codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#else
	void JsonParser::setJSONData(string_view jsonData, uint32_t codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#endif

	void JsonParser::setJSONData(string_view jsonData)
	{
		rawData = jsonData;

		this->parse();
	}

	void JsonParser::setJSONData(istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JsonParser::setJSONData(istream&& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JsonParser::setJSONData(string&& jsonData)
	{
		rawData = move(jsonData);

		this->parse();
	}

	ConstJSONIterator JsonParser::begin() const noexcept
	{
		return parsedData.begin();
	}

	ConstJSONIterator JsonParser::end() const noexcept
	{
		return parsedData.end();
	}

	const string& JsonParser::getRawData() const
	{
		return rawData;
	}

	const string& JsonParser::operator * () const
	{
		return rawData;
	}

	nullptr_t JsonParser::getNull(string_view key, bool recursive) const
	{
		return this->get<nullptr_t>(key, recursive);
	}

	const string& JsonParser::getString(string_view key, bool recursive) const
	{
		return this->get<string>(key, recursive);
	}

	bool JsonParser::getBool(string_view key, bool recursive) const
	{
		return this->get<bool>(key, recursive);
	}

	int64_t JsonParser::getInt(string_view key, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jUInt64_t:
			return static_cast<int64_t>(std::get<uint64_t>(result->second));

		case utility::VariantTypeEnum::jDouble:
			return static_cast<int64_t>(std::get<double>(result->second));

		case utility::VariantTypeEnum::jString:
			return stoll(std::get<string>(result->second));
		}

		return std::get<int64_t>(result->second);
	}

	uint64_t JsonParser::getUnsignedInt(string_view key, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jInt64_t:
			return static_cast<uint64_t>(std::get<int64_t>(result->second));

		case utility::VariantTypeEnum::jDouble:
			return static_cast<uint64_t>(std::get<double>(result->second));

		case utility::VariantTypeEnum::jString:
			return stoull(std::get<string>(result->second));
		}

		return std::get<uint64_t>(result->second);
	}

	double JsonParser::getDouble(string_view key, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jInt64_t:
			return static_cast<double>(std::get<int64_t>(result->second));

		case utility::VariantTypeEnum::jUInt64_t:
			return static_cast<double>(std::get<uint64_t>(result->second));

		case utility::VariantTypeEnum::jString:
			return stod(std::get<string>(result->second));
		}

		return std::get<double>(result->second);
	}

	const vector<JsonObject>& JsonParser::getArray(string_view key, bool recursive) const
	{
		return this->get<vector<JsonObject>>(key);
	}

	const JsonObject& JsonParser::getObject(string_view key, bool recursive) const
	{
		return this->get<JsonObject>(key);
	}

	bool JsonParser::tryGetNull(string_view key, bool recursive) const
	{
		nullptr_t value;

		return this->tryGet(key, value, recursive);
	}

	bool JsonParser::tryGetString(string_view key, string& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JsonParser::tryGetBool(string_view key, bool& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JsonParser::tryGetInt(string_view key, int64_t& value, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jInt64_t:
			value = std::get<int64_t>(result->second);

			return true;

		case utility::VariantTypeEnum::jUInt64_t:
			value = static_cast<int64_t>(std::get<uint64_t>(result->second));

			return true;

		case utility::VariantTypeEnum::jDouble:
			value = static_cast<int64_t>(std::get<double>(result->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stoll(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonParser::tryGetUnsignedInt(string_view key, uint64_t& value, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jUInt64_t:
			value = std::get<uint64_t>(result->second);

			return true;

		case utility::VariantTypeEnum::jInt64_t:
			value = static_cast<uint64_t>(std::get<int64_t>(result->second));

			return true;

		case utility::VariantTypeEnum::jDouble:
			value = static_cast<uint64_t>(std::get<double>(result->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stoull(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonParser::tryGetDouble(string_view key, double& value, bool recursive) const
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			return false;
		}

		utility::VariantTypeEnum type = static_cast<utility::VariantTypeEnum>(result->second.index());

		switch (type)
		{
		case utility::VariantTypeEnum::jDouble:
			value = std::get<double>(result->second);

			return true;

		case utility::VariantTypeEnum::jInt64_t:
			value = static_cast<double>(std::get<int64_t>(result->second));

			return true;

		case utility::VariantTypeEnum::jUInt64_t:
			value = static_cast<double>(std::get<uint64_t>(result->second));

			return true;

		case utility::VariantTypeEnum::jString:
			value = stod(std::get<string>(result->second));

			return true;

		default:
			return false;
		}
	}

	bool JsonParser::tryGetArray(string_view key, vector<JsonObject>& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	bool JsonParser::tryGetObject(string_view key, JsonObject& value, bool recursive) const
	{
		return this->tryGet(key, value, recursive);
	}

	const JsonObject& JsonParser::getParsedData() const
	{
		return parsedData;
	}

	void JsonParser::getParsedData(JsonObject& object) noexcept
	{
		object = move(parsedData);
	}

	void JsonParser::overrideValue(string_view key, const VariantType& value, bool recursive)
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		const_cast<VariantType&>(result->second) = value;
	}

	void JsonParser::overrideValue(string_view key, VariantType&& value, bool recursive)
	{
		auto [result, success] = JsonParser::find(key, parsedData.data, recursive);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		const_cast<VariantType&>(result->second) = move(value);
	}

	JSON_API istream& operator >> (istream& inputStream, JsonParser& parser)
	{
		parser.setJSONData(inputStream);

		return inputStream;
	}

	JSON_API ostream& operator << (ostream& outputStream, const JsonParser& parser)
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

			utility::outputJsonType<JsonParser, utility::JsonArrayWrapper>(outputStream, start->second, ++check == end, offset);

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
