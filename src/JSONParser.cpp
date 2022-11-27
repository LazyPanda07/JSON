#include "JSONParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>
#include <queue>

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

	utility::jsonObject::variantType JSONParser::getValue(const string& value)
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

	void JSONParser::insertKeyValueData(string&& key, const string& value, utility::jsonObject*& ptr, vector<utility::jsonObject>* currentArray)
	{
		if (key.empty())
		{
			utility::jsonObject object;

			object.data.push_back({ ""s, JSONParser::getValue(value) });

			currentArray->push_back(move(object));
		}
		else
		{
			if (currentArray)
			{
				utility::jsonObject::variantType* object = JSONParser::findObject(*currentArray);

				if (object)
				{
					std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONObject)>(*object).data.push_back({ move(key), JSONParser::getValue(value) });
				}
				else
				{
					std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONObject)>(currentArray->back().data.back().second).data.push_back({ move(key), JSONParser::getValue(value) });
				}
			}
			else
			{
				ptr->data.push_back({ move(key), JSONParser::getValue(value) });
			}
		}
	}

	pair<vector<pair<string, JSONParser::variantType>>::const_iterator, bool> JSONParser::find(const string& key, const vector<pair<string, variantType>>& start)
	{
		auto it = std::find_if(start.begin(), start.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

		if (it != start.end())
		{
			return { it, true };
		}

		it = start.begin();
		auto end = start.end();

		while (it != end)
		{
			if (it->second.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONArray))
			{
				const vector<utility::jsonObject>& jsonArray = std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONArray)>(it->second);

				for (const auto& i : jsonArray)
				{
					auto result = find(key, i.data);

					if (result.second)
					{
						return result;
					}
				}
			}
			else if (it->second.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONObject))
			{
				const vector<pair<string, variantType>>& data = ::get<utility::jsonObject>(it->second).data;

				auto result = find(key, data);

				if (result.second)
				{
					return result;
				}
			}

			++it;
		}

		return { end, false };
	}

	utility::jsonObject::variantType* JSONParser::findObject(const vector<utility::jsonObject>& currentArray)
	{
		if (currentArray.size() && currentArray.back().data.back().second.index() == utility::variantTypeEnum::jJSONObject)
		{
			const utility::jsonObject::variantType* object = &currentArray.back().data.back().second;

			while (object->index() == static_cast<size_t>(utility::variantTypeEnum::jJSONObject))
			{
				auto& data = std::get<json::utility::jsonObject>(*object).data;

				if (data.size() && data.back().second.index() == utility::variantTypeEnum::jJSONObject)
				{
					object = &data.back().second;
				}
				else
				{
					break;
				}
			}

			return const_cast<utility::jsonObject::variantType*>(object);
		}

		return nullptr;
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

		stack<pair<string, utility::jsonObject*>> dictionaries;
		stack<vector<utility::jsonObject>*> arrays;
		bool checkNestedObject = false;
		string key;
		string value;
		bool startString = false;
		bool escapeSymbol = false;

		for (const char& i : rawData)
		{
			if (!escapeSymbol && i == '\\')
			{
				escapeSymbol = true;

				continue;
			}
			else if (escapeSymbol)
			{
				escapeSymbol = false;

				value += JSONParser::interpretEscapeSymbol(i);

				continue;
			}

			if (!startString && isStringSymbol(i))
			{
				startString = true;
			}
			else if (startString && isStringSymbol(i))
			{
				startString = false;
			}

			if (startString)
			{
				value += i;

				continue;
			}

			if (isspace(static_cast<unsigned char>(i)))
			{
				continue;
			}

			switch (i)
			{
			case openCurlyBracket:
				if (dictionaries.empty())
				{
					dictionaries.push({ ""s, &parsedData });
				}
				else if (arrays.size())
				{
					vector<pair<string, jsonObject::variantType>>* newlyObject = nullptr;
					jsonObject::variantType* object = JSONParser::findObject(*arrays.top());

					if (object && checkNestedObject)
					{
						newlyObject = &std::get<jsonObject>(*object).data;
					}
					else
					{
						newlyObject = &arrays.top()->emplace_back().data;
					}

					newlyObject->push_back({ move(key), jsonObject() });
				}
				else
				{
					dictionaries.push({ move(key), new jsonObject() });
				}

				checkNestedObject = true;

				break;

			case closeCurlyBracket:
				checkNestedObject = false;

				if (value.size())
				{
					JSONParser::insertKeyValueData(move(key), value, dictionaries.top().second, arrays.size() ? arrays.top() : nullptr);

					value.clear();
				}

				{
					pair<string, jsonObject*> ptr = dictionaries.top();

					if (arrays.size())
					{
						continue;
					}

					dictionaries.pop();

					if (ptr.second != &parsedData)
					{
						dictionaries.top().second->data.push_back({ move(ptr.first), jsonObject(*ptr.second) });

						delete ptr.second;
					}
				}

				break;

			case openSquareBracket:
				if (arrays.size() && key.empty())
				{
					auto& currentArray = arrays.top();

					jsonObject object;

					auto& newArray = object.data.emplace_back(make_pair(""s, vector<jsonObject>())).second;

					arrays.push(&std::get<static_cast<size_t>(variantTypeEnum::jJSONArray)>(newArray));

					currentArray->push_back(move(object));
				}
				else if (arrays.size())
				{
					auto& newArray = std::get<jsonObject>(*JSONParser::findObject(*arrays.top())).data.emplace_back(make_pair(move(key), vector<jsonObject>())).second;

					arrays.push(&std::get<static_cast<size_t>(variantTypeEnum::jJSONArray)>(newArray));
				}
				else
				{
					if (dictionaries.empty())
					{
						dictionaries.push({ ""s, &parsedData });
					}

					auto& newArray = dictionaries.top().second->data.emplace_back(make_pair(move(key), vector<jsonObject>())).second;

					arrays.push(&std::get<static_cast<size_t>(variantTypeEnum::jJSONArray)>(newArray));
				}

				break;

			case closeSquareBracket:
				if (value.size())
				{
					JSONParser::insertKeyValueData(""s, value, dictionaries.top().second, arrays.size() ? arrays.top() : nullptr);

					value.clear();
				}

				arrays.pop();

				break;

			case comma:
				if (isNumber(value) || (value.size() && isStringSymbol(*value.begin()) && isStringSymbol(*value.rbegin())) || (value == "true" || value == "false" || value == "null"))
				{
					JSONParser::insertKeyValueData(move(key), value, dictionaries.top().second, arrays.size() ? arrays.top() : nullptr);

					value.clear();
				}
				else if (value.size())
				{
					value += i;
				}

				break;

			case colon:
				key = string(value.begin() + 1, value.end() - 1);

				value.clear();

				break;

			default:
				value += i;
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

	bool JSONParser::contains(const string& key, utility::variantTypeEnum type) const
	{
		queue<const utility::jsonObject*> objects;

		objects.push(&parsedData);

		while (objects.size())
		{
			const utility::jsonObject* current = objects.front();

			objects.pop();

			for (const auto& i : current->data)
			{
				if (i.first == key && i.second.index() == static_cast<size_t>(type))
				{
					return true;
				}

				if (i.second.index() == static_cast<size_t>(utility::variantTypeEnum::jJSONObject))
				{
					const auto& object = std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONObject)>(i.second);

					objects.push(&object);
				}
			}
		}

		return false;
	}

	void JSONParser::setJSONData(const string& jsonData, uint32_t codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}

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
	JSON_API const nullptr_t& JSONParser::get<nullptr_t>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<nullptr_t>(result->second);
	}

	template<>
	JSON_API const string& JSONParser::get<string>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<string>(result->second);
	}

	template<>
	JSON_API const bool& JSONParser::get<bool>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<bool>(result->second);
	}

	template<>
	JSON_API const int64_t& JSONParser::get<int64_t>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<int64_t>(result->second);
	}

	template<>
	JSON_API const uint64_t& JSONParser::get<uint64_t>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<uint64_t>(result->second);
	}

	template<>
	JSON_API const double& JSONParser::get<double>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<double>(result->second);
	}

	template<>
	JSON_API const vector<utility::jsonObject>& JSONParser::get<vector<utility::jsonObject>>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<vector<utility::jsonObject>>(result->second);
	}

	template<>
	JSON_API const utility::jsonObject& JSONParser::get<utility::jsonObject>(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

		if (!success)
		{
			throw exceptions::CantFindValueException(key);
		}

		return std::get<utility::jsonObject>(result->second);
	}

	nullptr_t JSONParser::getNull(const string& key) const
	{
		return this->get<nullptr_t>(key);
	}

	const string& JSONParser::getString(const string& key) const
	{
		return this->get<string>(key);
	}

	bool JSONParser::getBool(const string& key) const
	{
		return this->get<bool>(key);
	}

	int64_t JSONParser::getInt(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

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

	uint64_t JSONParser::getUnsignedInt(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

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

	double JSONParser::getDouble(const string& key) const
	{
		auto [result, success] = find(key, parsedData.data);

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

	const vector<utility::jsonObject>& JSONParser::getArray(const string& key) const
	{
		return this->get<vector<utility::jsonObject>>(key);
	}

	const utility::jsonObject& JSONParser::getObject(const string& key) const
	{
		return this->get<utility::jsonObject>(key);
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

		for (const auto& i : source)
		{
			if (i == '.')
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
