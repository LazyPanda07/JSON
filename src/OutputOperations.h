#pragma once

#include "JsonArrayWrapper.h"

namespace json::utility
{
	/// <summary>
	/// Put JSON value into outputStream
	/// </summary>
	/// <typeparam name="jsonStructT">last argument in baseVariantType</typeparam>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="value">JSON value</param>
	/// <param name="isLast">is description ends</param>
	std::ostream& outputJsonType(std::ostream& outputStream, const JsonObject& value, bool isLast, std::string& offset);

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

		const std::vector<JsonObject>& jsonArray = *jsonData;

		if (!jsonData.getOffset())
		{
			throw std::runtime_error("JsonArrayWrapper offset was nullptr");
		}

		std::string& offset = *jsonData.getOffset();

		for (size_t i = 0; i < jsonArray.size(); i++)
		{
			outputStream << offset;

			outputJsonType(outputStream, jsonArray[i], i + 1 == jsonArray.size(), offset);
		}

		return outputStream;
	}

	/// @brief Output JSON value to std::ostream. Also apply std::endl and flush
	/// @param outputStream std::ostream subclass
	/// @param jsonValue JSON value
	/// @return outputStream
	inline std::ostream& operator << (std::ostream& outputStream, const JsonObject& jsonValue)
	{
		std::string offset;

		outputJsonType(outputStream, jsonValue, true, offset);

		return outputStream;
	}
}
