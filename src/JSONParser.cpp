#include "JSONParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>

#define INSERT_DATA(key, value) if(isArrayData) { insertDataIntoArray(key, value, ptr); } else { ptr->data.insert(make_pair(move(key), value)); }

#define GET_METHOD(templateType) template<> const templateType& JSONParser::get<templateType>(const string& key) const { return ::get<templateType>(find(key, parsedData.data)->second); }

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
	using ConstIterator = JSONParser::ConstIterator;

	ConstIterator::ConstIterator(const ConstIterator& other) :
		begin(other.begin),
		end(other.end),
		current(other.current)
	{

	}

	ConstIterator::ConstIterator(ConstIteratorType begin, ConstIteratorType end, ConstIteratorType start) :
		begin(begin),
		end(end),
		current(start)
	{

	}

	ConstIterator ConstIterator::operator++ (int) noexcept
	{
		ConstIterator it(*this);

		++current;

		return it;
	}

	const ConstIterator& ConstIterator::operator++ () noexcept
	{
		++current;

		return *this;
	}

	ConstIterator ConstIterator::operator-- (int) noexcept
	{
		ConstIterator it(*this);

		--current;

		return it;
	}

	const ConstIterator& ConstIterator::operator-- () noexcept
	{
		--current;

		return *this;
	}

	const ConstIterator::ConstIteratorType& ConstIterator::operator * () const noexcept
	{
		return current;
	}

	const ConstIterator::ConstIteratorType& ConstIterator::operator -> () const noexcept
	{
		return **this;
	}

	bool ConstIterator::operator == (const ConstIterator& other) const noexcept
	{
		return current == other.current;
	}

	bool ConstIterator::operator != (const ConstIterator& other) const noexcept
	{
		return current != other.current;
	}

	template<typename T>
	void JSONParser::insertDataIntoArray(const string& key, T&& value, jsonStruct*& ptr)
	{
		try
		{
			::get<vector<T>>(ptr->data.at(key)).push_back(move(value));
		}
		catch (const bad_variant_access&)
		{
			ptr->data.at(key).emplace<vector<T>>(vector<T>({ move(value) }));
		}
	}

	void JSONParser::insertData(string&& key, const string& value, jsonStruct*& ptr)
	{
		bool isArrayData = ptr->data.find(key) != ptr->data.end();

		if (*value.begin() == '"' && *value.rbegin() == '"')
		{
			INSERT_DATA(key, string(value.begin() + 1, value.end() - 1));
		}
		else if (value == "true" || value == "false")
		{
			INSERT_DATA(key, value == "true" ? true : false);
		}
		else if (value == "null")
		{
			INSERT_DATA(key, nullptr);
		}
		else if (isNumber(value))
		{
			if (value.find('.') != string::npos)
			{
				INSERT_DATA(key, stod(value));
			}
			else
			{
				if (value.find('-'))
				{
					INSERT_DATA(key, stoll(value));
				}
				else if (uint64_t valueToInsert = stoull(value) > numeric_limits<int64_t>::max())
				{
					INSERT_DATA(key, move(valueToInsert));
				}
				else
				{
					INSERT_DATA(key, stoll(value));
				}
			}
		}
	}

	unordered_map<string, JSONParser::jsonStruct::variantType>::const_iterator JSONParser::find(const string& key, const unordered_map<string, jsonStruct::variantType>& start)
	{
		auto it = start.find(key);

		if (it != start.end())
		{
			return it;
		}

		it = start.begin();
		auto end = start.end();

		while (it != end)
		{
			if (it->second.index() == 16)
			{
				const unordered_map<string, jsonStruct::variantType>& data = ::get<unique_ptr<jsonStruct>>(it->second)->data;

				auto result = find(key, data);

				if (result != data.end())
				{
					return result;
				}
			}

			++it;
		}

		throw runtime_error("Wrong key");
	}

	void JSONParser::parse()
	{
		stack<pair<string, jsonStruct*>> maps;
		string key;
		string value;
		pair<string, jsonStruct*> ptr = { "", nullptr };
		bool startString = false;

		for (const auto& i : rawData)
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
					insertData(move(key), value, maps.top().second);

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
				maps.top().second->data.insert(make_pair(key, jsonStruct::variantType()));

				break;

			case closeSquareBracket:
				if (value.size())
				{
					insertData(move(key), value, maps.top().second);

					key.clear();
					value.clear();
				}

				break;

			case comma:
				if (isNumber(value) || (value.size() && *value.begin() == '"' && *value.rbegin() == '"') || (value == "true" || value == "false" || value == "null"))
				{
					insertData(move(key), value, maps.top().second);

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
		this->parse();
	}

	ConstIterator JSONParser::begin() const noexcept
	{
		return JSONParser::ConstIterator(parsedData.data.cbegin(), parsedData.data.cend(), parsedData.data.cbegin());
	}

	ConstIterator JSONParser::end() const noexcept
	{
		return JSONParser::ConstIterator(parsedData.data.cbegin(), parsedData.data.cend(), parsedData.data.cend());
	}

	const string& JSONParser::getRawData() const
	{
		return rawData;
	}

	const string& JSONParser::operator * () const
	{
		return rawData;
	}

	GET_METHOD(nullptr_t);

	GET_METHOD(string);

	GET_METHOD(char);

	GET_METHOD(unsigned char);

	GET_METHOD(bool);

	GET_METHOD(int64_t);

	GET_METHOD(uint64_t);

	GET_METHOD(double);

	GET_METHOD(vector<nullptr_t>);

	GET_METHOD(vector<string>);

	GET_METHOD(vector<char>);

	GET_METHOD(vector<unsigned char>);

	GET_METHOD(vector<bool>);

	GET_METHOD(vector<int64_t>);

	GET_METHOD(vector<uint64_t>);

	GET_METHOD(vector<double>);

	GET_METHOD(unique_ptr<JSONParser::jsonStruct>);

	istream& operator >> (istream& stream, JSONParser& parser)
	{
		string data;
		string tem;

		while (getline(stream, tem))
		{
			data += tem + '\n';
		}

		parser.rawData = data;

		parser.parse();

		return stream;
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
