#include "JsonBuilder.h"

#include <algorithm>
#include <format>

#include "JsonArrayWrapper.h"
#include "OutputOperations.h"
#include "Exceptions/CantFindValueException.h"

#ifndef __LINUX__
#pragma warning(disable: 4715)
#endif

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

	JsonObject& JsonBuilder::operator [](std::string_view key)
	{
		auto it = builderData.begin();

		for (; it != builderData.end(); ++it)
		{
			const JsonObject& value = *it;

			if (value.is<JsonObject>())
			{
				if (it.key() == key)
				{
					break;
				}
			}
		}

		if (it != builderData.end())
		{
			return *it;
		}

		return builderData[key];
	}

	const JsonObject& JsonBuilder::operator [](std::string_view key) const
	{
		auto it = builderData.begin();

		for (; it != builderData.end(); ++it)
		{
			const JsonObject& value = *it;

			if (value.is<JsonObject>())
			{
				if (it.key() == key)
				{
					break;
				}
			}
		}

		if (it == builderData.end())
		{
			throw exceptions::CantFindValueException(key);
		}

		return *it;
	}

	std::string JsonBuilder::build() const
	{
		JsonObject::ConstIterator start = builderData.begin();
		JsonObject::ConstIterator end = builderData.end();
		std::ostringstream outputStream;
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
				else if (std::isspace(result[i]) && !isJsonString)
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
