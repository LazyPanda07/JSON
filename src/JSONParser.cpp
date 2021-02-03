#include "JSONParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>

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
	void JSONParser::insertKeyValue(string&& key, const string& value, jsonStruct*& ptr)
	{
		if (*value.begin() == '"' && *value.rbegin() == '"')
		{
			ptr->data.insert(make_pair(move(key), string(value.begin() + 1, value.end() - 1)));
		}
		else if (bool logicValue = (value == "true" || value == "false"))
		{
			ptr->data.insert(make_pair(move(key), logicValue));
		}
		else if (value == "null")
		{
			ptr->data.insert(make_pair(move(key), nullptr));
		}
		else if (isNumber(value))
		{
			if (value.find('.') != string::npos)
			{
				//TODO: min check
				double valueToInsert = stod(value);

				if (numeric_limits<float>::max() > valueToInsert)
				{
					ptr->data.insert(make_pair(move(key), static_cast<float>(valueToInsert)));
				}
				else
				{
					ptr->data.insert(make_pair(move(key), valueToInsert));
				}
			}
			else
			{
				//TODO: min check
				int64_t valueToInsert = stoll(value);

				if (numeric_limits<int>::max() > valueToInsert)
				{
					ptr->data.insert(make_pair(move(key), static_cast<int>(valueToInsert)));
				}
				else
				{
					ptr->data.insert(make_pair(move(key), valueToInsert));
				}
			}
		}
	}

	void JSONParser::parse(const string& data)
	{
		stack<pair<string, jsonStruct*>> maps;
		string key;
		string value;
		pair<string, jsonStruct*> ptr = { "", nullptr };
		bool startString = false;

		for (const auto& i : data)
		{
			if (!startString && i == '"')
			{
				startString = true;
			}
			else if (startString && i == '"')
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
				if (maps.empty())
				{
					maps.push({ "", &parsedData });
				}
				else
				{
					maps.push(make_pair(move(key), new jsonStruct()));
				}

				break;

			case closeCurlyBracket:
				if (value.size())
				{
					insertKeyValue(move(key), value, maps.top().second);

					value.clear();
				}

				ptr = maps.top();

				maps.pop();

				if (ptr.second != &parsedData)
				{
					maps.top().second->data.insert(make_pair(move(ptr.first), unique_ptr<jsonStruct>(ptr.second)));
				}

				break;

			case openSquareBracket:

				break;

			case closeSquareBracket:

				break;

			case comma:
				if (isNumber(value) || (value.size() && *value.begin() == '"' && *value.rbegin() == '"'))
				{
					insertKeyValue(move(key), value, maps.top().second);

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
		rawData(data)
	{
		this->parse(data);
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
