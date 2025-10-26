#include "JSONBuilder.h"

#include <queue>
#include <algorithm>

#include "Exceptions/CantFindValueException.h"

#pragma warning(disable: 4715)

using namespace std;

namespace json
{
#if defined(__LINUX__) || defined(__ANDROID__)
	JSONBuilder::JSONBuilder(string_view codePage, outputType type) :
		codePage(codePage),
		type(type)
	{

	}

	JSONBuilder::JSONBuilder(const utility::jsonObject& data, string_view codePage, outputType type) :
		builderData(data),
		codePage(codePage),
		type(type)
	{

	}
#else
	JSONBuilder::JSONBuilder(uint32_t codePage, OutputType type) :
		codePage(codePage),
		type(type)
	{

	}

	JSONBuilder::JSONBuilder(const utility::JsonObject& data, uint32_t codePage, OutputType type) :
		builderData(data),
		codePage(codePage),
		type(type)
	{

	}
#endif

	JSONBuilder::JSONBuilder(const JSONBuilder& other) :
		builderData(other.builderData),
		codePage(other.codePage),
		type(other.type)
	{

	}

	JSONBuilder::JSONBuilder(JSONBuilder&& other) noexcept :
		builderData(move(other.builderData)),
		codePage(other.codePage),
		type(other.type)
	{

	}

	JSONBuilder& JSONBuilder::operator = (const JSONBuilder& other)
	{
		builderData = other.builderData;
		codePage = other.codePage;
		type = other.type;

		return *this;
	}

	JSONBuilder& JSONBuilder::operator = (JSONBuilder&& other) noexcept
	{
		builderData = move(other.builderData);
		codePage = other.codePage;
		type = other.type;

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<nullptr_t>(const pair<string, nullptr_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<nullptr_t>(pair<string, nullptr_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<string>(const pair<string, string>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<string>(pair<string, string>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<bool>(const pair<string, bool>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<bool>(pair<string, bool>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<int64_t>(const pair<string, int64_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<int64_t>(pair<string, int64_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<uint64_t>(const pair<string, uint64_t>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<uint64_t>(pair<string, uint64_t>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<double>(const pair<string, double>& value)
	{
		builderData.data.push_back(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<double>(pair<string, double>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<vector<utility::JsonObject>>(pair<string, vector<utility::JsonObject>>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::JsonObject>(const pair<string, utility::JsonObject>& value)
	{
		builderData.data.emplace_back(value.first, value.second);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<utility::JsonObject>(pair<string, utility::JsonObject>&& value) noexcept
	{
		builderData.data.push_back(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<int>(const pair<string, int>& value)
	{
		this->push_back<int64_t>(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<int>(pair<string, int>&& value) noexcept
	{
		this->push_back<int64_t>(move(value));

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<uint32_t>(const pair<string, uint32_t>& value)
	{
		this->push_back<uint64_t>(value);

		return *this;
	}

	template<>
	JSON_API JSONBuilder& JSONBuilder::push_back<uint32_t>(pair<string, uint32_t>&& value) noexcept
	{
		this->push_back<uint64_t>(move(value));

		return *this;
	}

	JSONBuilder& JSONBuilder::appendNull(string_view key)
	{
		return this->append(key, nullptr);
	}

	JSONBuilder& JSONBuilder::appendString(string_view key, string_view value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendBool(string_view key, bool value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendInt(string_view key, int64_t value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendUnsignedInt(string_view key, uint64_t value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendDouble(string_view key, double value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendArray(string_view key, vector<utility::JsonObject>&& value)
	{
		return this->append(key, move(value));
	}

	JSONBuilder& JSONBuilder::appendArray(string_view key, const vector<utility::JsonObject>& value)
	{
		return this->append(key, value);
	}

	JSONBuilder& JSONBuilder::appendObject(string_view key, utility::JsonObject&& value)
	{
		return this->append(key, move(value));
	}

	JSONBuilder& JSONBuilder::appendObject(string_view key, const utility::JsonObject& value)
	{
		return this->append(key, value);
	}

	bool JSONBuilder::contains(string_view key, utility::VariantTypeEnum type, bool recursive) const
	{
		queue<const utility::JsonObject*> objects;

		objects.push(&builderData);

		while (objects.size())
		{
			const utility::JsonObject* current = objects.front();

			objects.pop();

			for (const auto& i : current->data)
			{
				if (i.first == key && i.second.index() == static_cast<size_t>(type))
				{
					return true;
				}

				if (recursive && i.second.index() == static_cast<size_t>(utility::VariantTypeEnum::jJSONObject))
				{
					const auto& object = get<static_cast<size_t>(utility::VariantTypeEnum::jJSONObject)>(i.second);

					objects.push(&object);
				}
			}
		}

		return false;
	}

	JSONBuilder::VariantType& JSONBuilder::operator [] (string_view key)
	{
		auto it = find_if(builderData.data.begin(), builderData.data.end(), [&key](const pair<string, VariantType>& value) { return value.first == key; });

		if (it != builderData.data.end())
		{
			return it->second;
		}

		this->push_back<nullptr_t>(make_pair(string(key.data(), key.size()), nullptr));

		return builderData.data.back().second;
	}

	const JSONBuilder::VariantType& JSONBuilder::operator [] (string_view key) const
	{
		auto it = find_if(builderData.data.begin(), builderData.data.end(), [&key](const pair<string, VariantType>& value) { return value.first == key; });

		if (it != builderData.data.end())
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
		string offset = "  ";

		outputStream << '{' << endl;

		while (start != end)
		{
			auto check = start;

			outputStream << offset << '"' << start->first << '"' << ": ";

			utility::outputJSONType(outputStream, start->second, ++check == end, offset);

			++start;
		}

		outputStream << '}';

		switch (string result; type)
		{
		case json::JSONBuilder::OutputType::standard:
			return json::utility::toUTF8JSON(outputStream.str(), codePage);

		case json::JSONBuilder::OutputType::minimize:
			result = outputStream.str();
			bool isJsonString = false;

			for (size_t i = 0; i < result.size(); i++)
			{
				if (result[i] == '\"')
				{
					if (result[i - 1] != '\\')
					{
						isJsonString = !isJsonString;
					}
				}
				else if (isspace(result[i]) && !isJsonString)
				{
					result.erase(result.begin() + i);

					i--;
				}
			}

			return json::utility::toUTF8JSON(result, codePage);
		}
	}

	void JSONBuilder::standard()
	{
		type = OutputType::standard;
	}

	void JSONBuilder::minimize()
	{
		type = OutputType::minimize;
	}

	const utility::JsonObject& JSONBuilder::getObject() const
	{
		return builderData;
	}

	void JSONBuilder::getObject(utility::JsonObject& object) noexcept
	{
		object = move(builderData);
	}

	JSON_API ostream& operator << (ostream& outputStream, const JSONBuilder& builder)
	{
		return outputStream << builder.build();
	}
}
