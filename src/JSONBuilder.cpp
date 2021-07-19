#include "JSONBuilder.h"

#include "Exceptions/CantFindValueException.h"

using namespace std;

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
			if (it->second.index() == static_cast<int>(utility::variantTypeEnum::jJSONObject))
			{
#ifdef JSON_DLL
				vector<pair<string, variantType>>& data = ::get<shared_ptr<utility::jsonObject>>(it->second)->data;
#else
				vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonObject>>(it->second)->data;
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
			if (it->second.index() == static_cast<int>(utility::variantTypeEnum::jJSONObject))
			{
#ifdef JSON_DLL
				const vector<pair<string, variantType>>& data = ::get<shared_ptr<utility::jsonObject>>(it->second)->data;
#else
				const vector<pair<string, variantType>>& data = ::get<unique_ptr<utility::jsonObject>>(it->second)->data;
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

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, nullptr_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, nullptr_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, string>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, string>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, int64_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, int64_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, uint64_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, uint64_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(const pair<string, double>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back(pair<string, double>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::jsonObject*>(const pair<string, utility::jsonObject*>& value)
	{
#ifdef JSON_DLL
		builderData.data.push_back(make_pair(value.first, shared_ptr<utility::jsonObject>(value.second)));
#else
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonObject>(value.second)));
#endif // JSON_DLL

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::jsonObject*>(pair<string, utility::jsonObject*>&& value) noexcept
	{
#ifdef JSON_DLL
		builderData.data.push_back(make_pair(value.first, shared_ptr<utility::jsonObject>(value.second)));
#else
		builderData.data.push_back(make_pair(value.first, unique_ptr<utility::jsonObject>(value.second)));
#endif // JSON_DLL

		return *this;
	}

#ifdef JSON_DLL
	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<shared_ptr<utility::jsonObject>>(pair<string, shared_ptr<utility::jsonObject>>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}
#else
	template<>
	JSONBuilder& JSONBuilder::push_back<unique_ptr<utility::jsonObject>>(pair<string, unique_ptr<utility::jsonObject>>&& value) noexcept
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
		utility::jsonObject::offset = "  ";

		outputStream << "{\n";

		while (start != end)
		{
			auto check = start;

			outputStream << utility::jsonObject::offset << '"' << start->first << '"' << ": ";

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
