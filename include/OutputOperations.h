#pragma once

#include "JsonArrayWrapper.h"

namespace json::utility
{
	/// <summary>
	/// Put JSON value into outputStream
	/// </summary>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="value">JSON value</param>
	/// <param name="isLast">is description ends</param>
	/// <param name="offset">variable for saving current offset</param>
	std::ostream& outputJsonType(std::ostream& outputStream, const JsonObject& value, bool isLast, std::string& offset);

	/// <summary>
	/// Convert JSON value into std::string
	/// </summary>
	/// <param name="value">JSON value</param>
	/// <param name="isLast">is description ends</param>
	/// <param name="offset">variable for saving current offset</param>
	std::string outputJsonType(const JsonObject& value, bool isLast, std::string& offset);

	/// <summary>
	/// Output JSON arrays to std::ostream. Also applied std::endl and flushes
	/// </summary>
	/// <param name="outputStream">std::ostream subclass</param>
	/// <param name="jsonData">JSON array</param>
	/// <returns>outputStream</returns>
	/// <exception cref="std::runtime_error"></exception>
	std::ostream& operator << (std::ostream& outputStream, JsonArrayWrapper jsonData);

	/// @brief Output JSON value to std::ostream. Also apply std::endl and flush
	/// @param outputStream std::ostream subclass
	/// @param jsonValue JSON value
	/// @return outputStream
	std::ostream& operator << (std::ostream& outputStream, const JsonObject& jsonValue);
}
