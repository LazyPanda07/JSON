#include "JsonArrayWrapper.h"

namespace json::utility
{
	JsonArrayWrapper::JsonArrayWrapper(const std::vector<JsonObject>& array, std::string* offset) :
		array(array),
		offset(offset)
	{

	}

	JsonArrayWrapper::JsonArrayWrapper(const JsonObject& array, std::string* offset) :
		array(array.get<std::vector<JsonObject>>()),
		offset(offset)
	{

	}

	size_t JsonArrayWrapper::size() const
	{
		return array.size();
	}

	const JsonObject& JsonArrayWrapper::at(size_t index) const
	{
		return array.at(index);
	}

	const std::string* JsonArrayWrapper::getOffset() const
	{
		return offset;
	}

	std::string* JsonArrayWrapper::getOffset()
	{
		return offset;
	}

	const std::vector<JsonObject>& JsonArrayWrapper::operator *() const
	{
		return array;
	}

	const JsonObject& JsonArrayWrapper::operator [](size_t index) const
	{
		return array[index];
	}
}
