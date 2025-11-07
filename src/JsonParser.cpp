#include "JsonParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>
#include <limits>
#include <regex>
#include <format>

#include "JsonArrayWrapper.h"
#include "OutputOperations.h"

#ifndef __LINUX__
#pragma warning(disable: 4715)
#pragma warning(disable: 26800)
#endif

constexpr char openCurlyBracket = '{';
constexpr char closeCurlyBracket = '}';
constexpr char openSquareBracket = '[';
constexpr char closeSquareBracket = ']';
constexpr char comma = ',';
constexpr char colon = ':';

static bool isNumber(std::string_view source);

static bool isStringSymbol(char symbol);

std::pair<std::string, json::JsonObject> pop(std::stack<std::pair<std::string, json::JsonObject>>& processingData);

namespace json
{
	using ConstJSONIterator = JsonObject::Iterator;

	utility::JsonVariantType<JsonObject> JsonParser::parseValue(const std::string& value)
	{
#ifndef __LINUX__
#pragma warning(push)
#pragma warning(disable: 4018)
#endif

		if (isStringSymbol(*value.begin()) && isStringSymbol(*value.rbegin()))
		{
			return std::string(value.begin() + 1, value.end() - 1);
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
			if (value.find('.') != std::string::npos)
			{
				return std::stod(value);
			}
			else
			{
				if (value.find('-') != std::string::npos)
				{
					return std::stoll(value);
				}
				else if (uint64_t valueToInsert = stoull(value); valueToInsert > (std::numeric_limits<int64_t>::max)())
				{
					return valueToInsert;
				}
				else
				{
					return std::stoll(value);
				}
			}
		}

		throw std::runtime_error("Can't parse value");

		return nullptr;
#ifndef __LINUX__
#pragma warning(pop)
#endif
	}

	void JsonParser::insertValue(std::string_view key, const std::string& value, JsonObject& object)
	{
		object[key] = JsonParser::parseValue(value);
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

		static const std::regex singleLine(R"(//[^\n\r]*)", std::regex_constants::ECMAScript);
		static const std::regex multiLine(R"(/\*([^*]|\*+[^*/])*\*+/)", std::regex_constants::ECMAScript);

		rawData = std::regex_replace(rawData, singleLine, ""); // Remove // comments
		rawData = std::regex_replace(rawData, multiLine, ""); // Remove /* ... */ comments (including multiline)

		std::stack<std::pair<std::string, JsonObject>> processingData;
		std::string key;
		std::string value;
		bool startString = false;
		bool escapeSymbol = false;
		size_t index = 0;

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
				processingData.emplace(std::move(key), JsonObject::createDefaultJsonObject<utility::JsonVariantTypeEnum::jJSONObject>());

				break;

			case closeCurlyBracket:
			{
				auto [topKey, top] = pop(processingData);

				if (value.size())
				{
					top[std::move(key)] = JsonParser::parseValue(value);

					value.clear();
				}

				if (processingData.size())
				{
					if (topKey.size())
					{
						processingData.top().second[std::move(topKey)] = std::move(top);
					}
					else
					{
						processingData.top().second.emplace_back(std::move(top));
					}
				}
				else
				{
					parsedData = std::move(top);
				}
			}

			break;

			case openSquareBracket:
				processingData.emplace(std::move(key), JsonObject::createDefaultJsonObject<utility::JsonVariantTypeEnum::jJSONArray>());

				break;

			case closeSquareBracket:
			{
				auto [topKey, top] = pop(processingData);

				if (value.size())
				{
					top.emplace_back(JsonObject(JsonParser::parseValue(value)));

					value.clear();
				}

				if (topKey.size())
				{
					processingData.top().second[std::move(topKey)] = std::move(top);
				}
				else
				{
					processingData.top().second.emplace_back(std::move(top));
				}
			}

			break;

			case comma:
				if (isNumber(value) || (value.size() && isStringSymbol(*value.begin()) && isStringSymbol(*value.rbegin())) || (value == "true" || value == "false" || value == "null"))
				{
					if (processingData.top().second.is<json::JsonObject>())
					{
						processingData.top().second[std::move(key)] = JsonParser::parseValue(value);
					}
					else if (processingData.top().second.is<std::vector<json::JsonObject>>())
					{
						processingData.top().second.emplace_back(JsonObject(JsonParser::parseValue(value)));
					}
					else
					{
						throw std::runtime_error(std::format("Can't add value to {}", processingData.top().second.getType().name()));
					}

					value.clear();
				}
				else if (value.size())
				{
					value += c;
				}

				break;

			case colon:
				key = std::string(value.begin() + 1, value.end() - 1);

				value.clear();

				break;

			default:
				value += c;
			}
		}
	}

	JsonObject* JsonParser::find(std::string_view key, bool recursive)
	{
		std::queue<JsonObject*> objects;

		objects.push(&parsedData);

		while (objects.size())
		{
			JsonObject* current = objects.front();
			bool isObject = current->is<JsonObject>();

			objects.pop();

			for (auto it = current->begin(); it != current->end(); ++it)
			{
				JsonObject& value = *it;

				if (isObject)
				{
					if (it.key() == key)
					{
						return &value;
					}
				}

				if (recursive && value.is<JsonObject>() || value.is<std::vector<JsonObject>>())
				{
					objects.push(&value);
				}
			}
		}

		return nullptr;
	}

	const JsonObject* JsonParser::find(std::string_view key, bool recursive) const
	{
		std::queue<const JsonObject*> objects;

		objects.push(&parsedData);

		while (objects.size())
		{
			const JsonObject* current = objects.front();
			bool isObject = current->is<JsonObject>();

			objects.pop();

			for (auto it = current->begin(); it != current->end(); ++it)
			{
				const JsonObject& value = *it;

				if (isObject)
				{
					if (it.key() == key)
					{
						return &value;
					}
				}

				if (recursive && value.is<JsonObject>() || value.is<std::vector<JsonObject>>())
				{
					objects.push(&value);
				}
			}
		}

		return nullptr;
	}

	JsonParser::JsonParser(std::string_view data) :
		rawData(utility::toUTF8JSON(data, CP_UTF8))
	{
		this->parse();
	}

	JsonParser::JsonParser(std::istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((std::ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	JsonParser::JsonParser(std::istream&& inputStream) :
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
		rawData(std::move(other.rawData)),
		parsedData(std::move(other.parsedData))
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
		rawData = std::move(other.rawData);
		parsedData = std::move(other.parsedData);

		return *this;
	}

#if defined(__LINUX__) || defined(__ANDROID__)
	void JsonParser::setJSONData(std::string_view jsonData, std::string_view codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#else
	void JsonParser::setJSONData(std::string_view jsonData, uint32_t codePage)
	{
		rawData = utility::toUTF8JSON(jsonData, codePage);

		this->parse();
	}
#endif

	void JsonParser::setJSONData(std::string_view jsonData)
	{
		rawData = jsonData;

		this->parse();
	}

	void JsonParser::setJSONData(std::istream& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((std::ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JsonParser::setJSONData(std::istream&& inputStream)
	{
		if (!inputStream.good())
		{
			throw exceptions::WrongInputStreamException(inputStream);
		}

		rawData = utility::toUTF8JSON((std::ostringstream() << inputStream.rdbuf()).str(), CP_UTF8);

		this->parse();
	}

	void JsonParser::setJSONData(std::string&& jsonData)
	{
		rawData = std::move(jsonData);

		this->parse();
	}

	JsonObject::ConstIterator JsonParser::begin() const noexcept
	{
		return parsedData.begin();
	}

	JsonObject::ConstIterator JsonParser::end() const noexcept
	{
		return parsedData.end();
	}

	const std::string& JsonParser::getRawData() const
	{
		return rawData;
	}

	const std::string& JsonParser::operator * () const
	{
		return rawData;
	}

	const JsonObject& JsonParser::getParsedData() const
	{
		return parsedData;
	}

	void JsonParser::getParsedData(JsonObject& object) noexcept
	{
		object = std::move(parsedData);
	}

	std::istream& operator >> (std::istream& inputStream, JsonParser& parser)
	{
		parser.setJSONData(inputStream);

		return inputStream;
	}

	std::ostream& operator << (std::ostream& outputStream, const JsonParser& parser)
	{
		JsonObject::ConstIterator start = parser.begin();
		JsonObject::ConstIterator end = parser.end();
		std::string offset = "  ";

		outputStream << '{' << std::endl;

		while (start != end)
		{
			JsonObject::ConstIterator check = start;
			const JsonObject& value = *check;
			
			if (std::optional<std::string_view> key = check.key())
			{
				outputStream << std::format(R"({}"{}": )", offset, *key);
			}
			else
			{
				outputStream << offset;
			}

			utility::outputJsonType(outputStream, value, ++check == end, offset);

			++start;
		}

		outputStream << '}';

		return outputStream;
	}
}

bool isNumber(std::string_view source)
{
	if (source.empty())
	{
		return false;
	}

	if (std::all_of(source.begin(), source.end(), [](char c) { constexpr std::string_view symbols = "0123456789-."; return std::find(symbols.begin(), symbols.end(), c) != symbols.end(); }))
	{
		size_t check = 0;

		for (char c : source)
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

bool isStringSymbol(char symbol)
{
	return symbol == '"';
}

std::pair<std::string, json::JsonObject> pop(std::stack<std::pair<std::string, json::JsonObject>>& processingData)
{
	std::pair<std::string, json::JsonObject> result = std::move(processingData.top());

	processingData.pop();

	return result;
}
