#include "JsonBuilder.h"

#include <queue>
#include <algorithm>

#include "JsonArrayWrapper.h"
#include "OutputOperations.h"
#include "Exceptions/CantFindValueException.h"

#pragma warning(disable: 4715)

namespace json
{
#if defined(__LINUX__) || defined(__ANDROID__)
	JsonBuilder::JsonBuilder(std::string_view codePage, OutputType type) :
		codePage(codePage),
		type(type)
	{

	}

	JsonBuilder::JsonBuilder(const JsonObject& data, std::string_view codePage, OutputType type) :
		builderData(data),
		codePage(codePage),
		type(type)
	{

	}
#else
	JsonBuilder::JsonBuilder(uint32_t codePage, OutputType type) :
		codePage(codePage),
		type(type)
	{

	}

	JsonBuilder::JsonBuilder(const JsonObject& data, uint32_t codePage, OutputType type) :
		builderData(data),
		codePage(codePage),
		type(type)
	{

	}
#endif

	JsonBuilder::JsonBuilder(const JsonBuilder& other) :
		builderData(other.builderData),
		codePage(other.codePage),
		type(other.type)
	{

	}

	JsonBuilder::JsonBuilder(JsonBuilder&& other) noexcept :
		builderData(std::move(other.builderData)),
		codePage(other.codePage),
		type(other.type)
	{

	}

	JsonBuilder& JsonBuilder::operator = (const JsonBuilder& other)
	{
		builderData = other.builderData;
		codePage = other.codePage;
		type = other.type;

		return *this;
	}

	JsonBuilder& JsonBuilder::operator = (JsonBuilder&& other) noexcept
	{
		builderData = std::move(other.builderData);
		codePage = other.codePage;
		type = other.type;

		return *this;
	}

	bool JsonBuilder::contains(std::string_view key, utility::VariantTypeEnum type, bool recursive) const
	{
		std::queue<const JsonObject*> objects;

		objects.push(&builderData);

		while (objects.size())
		{
			const JsonObject* current = objects.front();

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

	JsonBuilder::VariantType& JsonBuilder::operator [] (std::string_view key)
	{
		auto it = find_if(builderData.data.begin(), builderData.data.end(), [&key](const std::pair<std::string, VariantType>& value) { return value.first == key; });

		if (it != builderData.data.end())
		{
			return it->second;
		}

		this->append<nullptr_t>(std::string_view(key.data(), key.size()), nullptr);

		return builderData.data.back().second;
	}

	const JsonBuilder::VariantType& JsonBuilder::operator [] (std::string_view key) const
	{
		auto it = find_if(builderData.data.begin(), builderData.data.end(), [&key](const std::pair<std::string, VariantType>& value) { return value.first == key; });

		if (it != builderData.data.end())
		{
			return it->second;
		}

		throw exceptions::CantFindValueException(key);
	}

	std::string JsonBuilder::build() const
	{
		auto start = builderData.data.begin();
		auto end = builderData.data.end();
		std::ostringstream outputStream;
		std::string offset = "  ";

		outputStream << '{' << std::endl;

		while (start != end)
		{
			auto check = start;

			outputStream << offset << '"' << start->first << '"' << ": ";

			utility::outputJsonType<JsonBuilder, utility::JsonArrayWrapper>(outputStream, start->second, ++check == end, offset);

			++start;
		}

		outputStream << '}';

		switch (std::string result; type)
		{
		case json::JsonBuilder::OutputType::standard:
			return json::utility::toUTF8JSON(outputStream.str(), codePage);

		case json::JsonBuilder::OutputType::minimize:
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

	void JsonBuilder::standard()
	{
		type = OutputType::standard;
	}

	void JsonBuilder::minimize()
	{
		type = OutputType::minimize;
	}

	const JsonObject& JsonBuilder::getObject() const
	{
		return builderData;
	}

	void JsonBuilder::getObject(JsonObject& object) noexcept
	{
		object = std::move(builderData);
	}

	std::ostream& operator << (std::ostream& outputStream, const JsonBuilder& builder)
	{
		return outputStream << builder.build();
	}
}
