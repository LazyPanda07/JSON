#include "JSONBuilder.h"

#include <iostream>

using namespace std;

#define PUSH_BACK_METHOD(templateParameter) template<> void JSONBuilder::push_back(const pair<string, templateParameter>& value) { builderData.data.push_back(value); }	\
template<> void JSONBuilder::push_back(pair<string, templateParameter>&& value) noexcept { builderData.data.push_back(move(value)); }

namespace json
{
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
	void JSONBuilder::push_back<int>(const pair<string, int>& value)
	{
		this->push_back<int64_t>(value);
	}

	template<>
	void JSONBuilder::push_back<int>(pair<string, int>&& value) noexcept
	{
		this->push_back<int64_t>(move(value));
	}

	template<>
	void JSONBuilder::push_back<unsigned int>(const pair<string, unsigned int>& value)
	{
		this->push_back<uint64_t>(value);
	}

	template<>
	void JSONBuilder::push_back<unsigned int>(pair<string, unsigned int>&& value) noexcept
	{
		this->push_back<uint64_t>(move(value));
	}

	JSONBuilder::variantType& JSONBuilder::operator [] (const string& key)
	{
		return builderData.data.back().second;
	}

	const JSONBuilder::variantType& JSONBuilder::operator [] (const string& key) const
	{
		return builderData.data.back().second;
	}
}
