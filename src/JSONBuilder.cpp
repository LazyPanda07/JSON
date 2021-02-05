#include "JSONBuilder.h"

#include "Exceptions/CantFindValueException.h"

using namespace std;

#define PUSH_BACK_METHOD(templateParameter) template<> JSONBuilder& JSONBuilder::push_back(const pair<string, templateParameter>& value) { builderData.data.push_back(value); return *this; }	\
template<> JSONBuilder& JSONBuilder::push_back(pair<string, templateParameter>&& value) noexcept { builderData.data.push_back(move(value)); return *this; }

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
				vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonBuilderStruct>>(it->second)->data;

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
				const vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonBuilderStruct>>(it->second)->data;

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

	PUSH_BACK_METHOD(nullptr_t);

	PUSH_BACK_METHOD(string);

	PUSH_BACK_METHOD(char);

	PUSH_BACK_METHOD(unsigned char);

	PUSH_BACK_METHOD(bool);

	PUSH_BACK_METHOD(int64_t);

	PUSH_BACK_METHOD(uint64_t);

	PUSH_BACK_METHOD(double);

	PUSH_BACK_METHOD(vector<nullptr_t>);

	PUSH_BACK_METHOD(vector<string>);

	PUSH_BACK_METHOD(vector<char>);

	PUSH_BACK_METHOD(vector<unsigned char>);

	PUSH_BACK_METHOD(vector<bool>);

	PUSH_BACK_METHOD(vector<int64_t>);

	PUSH_BACK_METHOD(vector<uint64_t>);

	PUSH_BACK_METHOD(vector<double>);

	template<>
	JSONBuilder& JSONBuilder::push_back<utility::jsonBuilderStruct*>(const pair<string, utility::jsonBuilderStruct*>& value)
	{
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonBuilderStruct>(value.second)));

		return *this;
	}

	template<>
	JSONBuilder& JSONBuilder::push_back<utility::jsonBuilderStruct*>(pair<string, utility::jsonBuilderStruct*>&& value) noexcept
	{
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonBuilderStruct>(value.second)));

		return *this;
	}

	template<>
	JSONBuilder& JSONBuilder::push_back<unique_ptr<utility::jsonBuilderStruct>>(pair<string, unique_ptr<utility::jsonBuilderStruct>>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

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
}
