#include "JSONParser.h"

#include <stack>
#include <string>
#include <cctype>
#include <algorithm>

#include "Exceptions/CantFindValueException.h"

#undef max

#define GET_METHOD(templateType) template<> \
JSON_API const templateType& JSONParser::get<templateType>(const string& key) const \
{ \
	auto [result, success] = find(key, parsedData.data); \
	  \
	if(!success) \
	{ \
		throw exceptions::CantFindValueException(key); \
	} \
	  \
	return ::get<templateType>(result->second); \
}

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
				else if (uint64_t valueToInsert = stoull(value) > numeric_limits<int64_t>::max())
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

	void JSONParser::insertKeyValueData(string&& key, const string& value, utility::jsonObject*& ptr, vector<utility::objectSmartPointer<utility::jsonObject>>* currentArray)
	{
		if (key.empty())
		{
			utility::objectSmartPointer<utility::jsonObject> object(new utility::jsonObject());

			object->data.push_back({ ""s, JSONParser::getValue(value) });

			cout << value << '\t' << currentArray << endl;

			currentArray->push_back(move(object));
		}
		else
		{
			if (currentArray)
			{
				std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONObject)>(currentArray->back()->data.back().second)->data.push_back({ move(key), JSONParser::getValue(value) });
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
			if (it->second.index() == static_cast<int>(utility::variantTypeEnum::jJSONObject))
			{
				const vector<pair<string, variantType>>& data = ::get<utility::objectSmartPointer<utility::jsonObject>>(it->second)->data;

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

	bool JSONParser::isStringSymbol(char symbol)
	{
		return symbol == '"' || symbol == '\'';
	}

	void JSONParser::parse()
	{
		stack<pair<string, utility::jsonObject*>> dictionaries;
		stack<vector<utility::objectSmartPointer<utility::jsonObject>>*> arrays;
		string key;
		string value;
		pair<string, utility::jsonObject*> ptr = { "", nullptr };
		bool startString = false;

		for (const auto& i : rawData)
		{
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
					auto& newlyObject = arrays.top()->emplace_back(utility::objectSmartPointer<utility::jsonObject>(new utility::jsonObject()))->data;

					newlyObject.push_back({ ""s, utility::objectSmartPointer<utility::jsonObject>(new utility::jsonObject()) });
				}
				else
				{
					dictionaries.push({ move(key), new utility::jsonObject() });
				}

				break;

			case closeCurlyBracket:
				if (value.size())
				{
					JSONParser::insertKeyValueData(move(key), value, dictionaries.top().second, arrays.size() ? arrays.top() : nullptr);

					value.clear();
				}

				if (arrays.size())
				{
					continue;
				}

				ptr = dictionaries.top();

				dictionaries.pop();

				if (ptr.first.size())
				{
					dictionaries.top().second->data.push_back({ move(ptr.first), utility::objectSmartPointer<utility::jsonObject>(ptr.second) });
				}

				break;

			case openSquareBracket:
				if (arrays.size())
				{
					auto& currentArray = arrays.top();

					utility::objectSmartPointer<utility::jsonObject> object(new utility::jsonObject());

					auto& newArray = object->data.emplace_back(make_pair(""s, vector<utility::objectSmartPointer<utility::jsonObject>>())).second;

					arrays.push(&std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONArray)>(newArray));

					currentArray->push_back(move(object));
				}
				else
				{
					auto& newArray = dictionaries.top().second->data.emplace_back(make_pair(move(key), vector<utility::objectSmartPointer<utility::jsonObject>>())).second;

					arrays.push(&std::get<static_cast<size_t>(utility::variantTypeEnum::jJSONArray)>(newArray));
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

	JSONParser::JSONParser(ifstream& inputStream)
	{
		string line;
		string data;

		while (getline(inputStream, line))
		{
			data += line + '\n';
		}

		rawData = utility::toUTF8JSON(data, CP_UTF8);

		this->parse();
	}

	JSONParser::JSONParser(ifstream&& inputStream) :
		JSONParser(inputStream)
	{

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

	void JSONParser::setJSONData(string&& jsonData) noexcept
	{
		rawData = move(jsonData);

		this->parse();
	}

	ConstIterator JSONParser::begin() const noexcept
	{
		return ConstIterator(parsedData.data.cbegin(), parsedData.data.cend(), parsedData.data.cbegin());
	}

	ConstIterator JSONParser::end() const noexcept
	{
		return ConstIterator(parsedData.data.cbegin(), parsedData.data.cend(), parsedData.data.cend());
	}

	const string& JSONParser::getRawData() const
	{
		return rawData;
	}

	const string& JSONParser::operator * () const
	{
		return rawData;
	}

	// GET_METHOD(nullptr_t);
	// 
	// GET_METHOD(string);
	// 
	// GET_METHOD(bool);
	// 
	// GET_METHOD(int64_t);
	// 
	// GET_METHOD(uint64_t);
	// 
	// GET_METHOD(double);
	// 
	// GET_METHOD(vector<nullptr_t>);
	// 
	// GET_METHOD(vector<string>);
	// 
	// GET_METHOD(vector<bool>);
	// 
	// GET_METHOD(vector<int64_t>);
	// 
	// GET_METHOD(vector<uint64_t>);
	// 
	// GET_METHOD(vector<double>);

#ifdef JSON_DLL
	GET_METHOD(shared_ptr<utility::jsonObject>);
#else
	GET_METHOD(utility::objectSmartPointer<utility::jsonObject>);
#endif // JSON_DLL

	istream& operator >> (istream& inputStream, JSONParser& parser)
	{
		string data;
		string tem;

		while (getline(inputStream, tem))
		{
			data += tem + '\n';
		}

		parser.rawData = utility::toUTF8JSON(data, CP_UTF8);

		parser.parse();

		return inputStream;
	}

	ostream& operator << (ostream& outputStream, const JSONParser& parser)
	{
		auto start = parser.begin();
		auto end = parser.end();
		utility::jsonObject::offset = "  ";

		outputStream << "{\n";

		while (start != end)
		{
			auto check = start;

			if (start->first.size())
			{
				outputStream << utility::jsonObject::offset << '"' << start->first << '"' << ": ";
			}
			else
			{
				outputStream << utility::jsonObject::offset;
			}

			utility::outputJSONType(outputStream, start->second, ++check == end);

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
