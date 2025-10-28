#pragma once

#include "JsonArrayWrapper.h"

namespace json::utility
{
	/// <summary>
	/// Output JSON arrays to std::ostream. Also applied std::endl and flushes
	/// </summary>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="jsonData">JSON array</param>
	/// <returns>outputStream</returns>
	/// <exception cref="std::runtime_error"></exception>
	inline std::ostream& operator << (std::ostream& outputStream, JsonArrayWrapper jsonData)
	{
		outputStream << '[' << std::endl;

		auto& jsonArray = *jsonData;

		if (!jsonData.getOffset())
		{
			throw std::runtime_error("JsonArrayWrapper offset was nullptr");
		}

		std::string& offset = *jsonData.getOffset();

		for (size_t i = 0; i < jsonArray.size(); i++)
		{
			for (const auto& j : jsonArray[i].data)
			{
				outputStream << offset;

				outputJsonType<JsonObject, JsonArrayWrapper>(outputStream, j.second, i + 1 == jsonArray.size(), offset);
			}
		}

		return outputStream;
	}

	/// @brief Output JSON value to std::ostream. Also apply std::endl and flush
	/// @param outputStream std::ostream subclass
	/// @param jsonValue JSON value
	/// @return outputStream
	inline std::ostream& operator << (std::ostream& outputStream, const JsonObject::VariantType& jsonValue)
	{
		std::string offset;

		outputJsonType<JsonObject, JsonArrayWrapper>(outputStream, jsonValue, true, offset);

		return outputStream;
	}
}
