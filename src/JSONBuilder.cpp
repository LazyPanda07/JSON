#include "JSONBuilder.h"

#include "Exceptions/CantFindValueException.h"

using namespace std;

#define PUSH_BACK_METHOD(templateParameter) template<> JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, templateParameter>& value) { builderData.data.push_back(value); return *this; }	\
template<> JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, templateParameter>&& value) noexcept { builderData.data.push_back(move(value)); return *this; }

namespace json
{
	pair<vector<pair<string, JSONBuilder::variantType>>::iterator, bool> JSONBuilder::find(const string& key, vector<pair<string, variantType>>& start)
	{
		auto it = find_if(start.begin(), start.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

		if (it != start.end())
		{
			return { it, true };
		}

		it = start.begin();
		auto end = start.end();

		while (it != end)
		{
			if (it->second.index() == utility::variantTypeEnum::jJsonStruct)
			{
#ifdef JSON_DLL
				vector<pair<string, variantType>>& data = ::get<shared_ptr<utility::jsonBuilderStruct>>(it->second)->data;
#else
				vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonBuilderStruct>>(it->second)->data;
#endif // JSON_DLL

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

	pair<vector<pair<string, JSONBuilder::variantType>>::const_iterator, bool> JSONBuilder::find(const string& key, const vector<pair<string, variantType>>& start)
	{
		auto it = find_if(start.begin(), start.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

		if (it != start.end())
		{
			return { it, true };
		}

		it = start.begin();
		auto end = start.end();

		while (it != end)
		{
			if (it->second.index() == utility::variantTypeEnum::jJsonStruct)
			{
#ifdef JSON_DLL
				const vector<pair<string, variantType>>& data = ::get<shared_ptr<utility::jsonBuilderStruct>>(it->second)->data;
#else
				const vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonBuilderStruct>>(it->second)->data;
#endif // JSON_DLL

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

	JSONBuilder::JSONBuilder(unsigned int codepage, outputType type) :
		codepage(codepage),
		type(type)
	{

	}

	PUSH_BACK_METHOD(nullptr_t);

	PUSH_BACK_METHOD(string);

	PUSH_BACK_METHOD(bool);

	PUSH_BACK_METHOD(int64_t);

	PUSH_BACK_METHOD(uint64_t);

	PUSH_BACK_METHOD(double);

	PUSH_BACK_METHOD(vector<nullptr_t>);

	PUSH_BACK_METHOD(vector<string>);

	PUSH_BACK_METHOD(vector<bool>);

	PUSH_BACK_METHOD(vector<int64_t>);

	PUSH_BACK_METHOD(vector<uint64_t>);

	PUSH_BACK_METHOD(vector<double>);

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::jsonBuilderStruct*>(const pair<string, utility::jsonBuilderStruct*>& value)
	{
#ifdef JSON_DLL
		builderData.data.push_back(make_pair(value.first, shared_ptr<utility::jsonBuilderStruct>(value.second)));
#else
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonBuilderStruct>(value.second)));
#endif // JSON_DLL

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::jsonBuilderStruct*>(pair<string, utility::jsonBuilderStruct*>&& value) noexcept
	{
#ifdef JSON_DLL
		builderData.data.push_back(make_pair(value.first, shared_ptr<utility::jsonBuilderStruct>(value.second)));
#else
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonBuilderStruct>(value.second)));
#endif // JSON_DLL

		return *this;
	}

#ifdef JSON_DLL
	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<shared_ptr<utility::jsonBuilderStruct>>(pair<string, shared_ptr<utility::jsonBuilderStruct>>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}
#else
	template<>
	JSONBuilder& JSONBuilder::push_back<unique_ptr<utility::jsonBuilderStruct>>(pair<string, unique_ptr<utility::jsonBuilderStruct>>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}
#endif // JSON_DLL

	template<>
	JSONBuilder& JSONBuilder::push_back<int>(const pair<string, int>& value)
	{
		this->push_back<int64_t>(value);

		return *this;
	}

	template<>
	JSONBuilder& JSONBuilder::push_back<int>(pair<string, int>&& value) noexcept
	{
		this->push_back<int64_t>(move(value));

		return *this;
	}

	template<>
	JSONBuilder& JSONBuilder::push_back<unsigned int>(const pair<string, unsigned int>& value)
	{
		this->push_back<uint64_t>(value);

		return *this;
	}

	template<>
	JSONBuilder& JSONBuilder::push_back<unsigned int>(pair<string, unsigned int>&& value) noexcept
	{
		this->push_back<uint64_t>(move(value));

		return *this;
	}

	JSONBuilder::variantType& JSONBuilder::operator [] (const string& key)
	{
		auto [it, success] = find(key, builderData.data);

		if (success)
		{
			return it->second;
		}

		this->push_back<nullptr_t>(make_pair(key, nullptr));

		return builderData.data.back().second;
	}

	const JSONBuilder::variantType& JSONBuilder::operator [] (const string& key) const
	{
		auto [it, success] = find(key, builderData.data);

		if (success)
		{
			return it->second;
		}

		throw exceptions::CantFindValueException(key);
	}

	string JSONBuilder::build() const
	{
		auto start = builderData.data.begin();
		auto end = builderData.data.end();
		ostringstream outputStream;
		offset = "  ";

		outputStream << "{\n";

		while (start != end)
		{
			auto check = start;

			outputStream << offset << '"' << start->first << '"' << ": ";

			utility::outputJSONType(outputStream, start->second, ++check == end);

			++start;
		}

		outputStream << '}';

		switch (string result; type)
		{
		case json::JSONBuilder::outputType::standard:
			return json::utility::toUTF8JSON(outputStream.str(), codepage);

		case json::JSONBuilder::outputType::minimize:
			result = outputStream.str();

			result.erase(remove_if(result.begin(), result.end(), [](unsigned char c) { return isspace(c); }), result.end());

			return json::utility::toUTF8JSON(result, codepage);
		}
	}

	void JSONBuilder::standard()
	{
		type = outputType::standard;
	}

	void JSONBuilder::minimize()
	{
		type = outputType::minimize;
	}

	ostream& operator << (ostream& outputStream, const JSONBuilder& builder)
	{
		return outputStream << builder.build();
	}
}
