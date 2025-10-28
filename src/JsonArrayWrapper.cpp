#include "JsonArrayWrapper.h"

using namespace std;

namespace json
{
	namespace utility
	{
		JsonArrayWrapper::JsonArrayWrapper(const vector<JsonObject>& array, string* offset) :
			array(array),
			offset(offset)
		{

		}

		JsonArrayWrapper::JsonArrayWrapper(const JsonObject::VariantType& array, string* offset) :
			array(get<vector<JsonObject>>(array)),
			offset(offset)
		{

		}

		size_t JsonArrayWrapper::size() const
		{
			return array.size();
		}

		nullptr_t JsonArrayWrapper::getNull(size_t index) const
		{
			return get<nullptr_t>(array.at(index).data.front().second);
		}

		string JsonArrayWrapper::getString(size_t index) const
		{
			return get<string>(array.at(index).data.front().second);
		}

		bool JsonArrayWrapper::getBool(size_t index) const
		{
			return get<bool>(array.at(index).data.front().second);
		}

		int64_t JsonArrayWrapper::getInt64_t(size_t index) const
		{
			return get<int64_t>(array.at(index).data.front().second);
		}

		uint64_t JsonArrayWrapper::getUInt64_t(size_t index) const
		{
			return get<uint64_t>(array.at(index).data.front().second);
		}

		double JsonArrayWrapper::getDouble(size_t index) const
		{
			return get<double>(array.at(index).data.front().second);
		}

		const JsonObject& JsonArrayWrapper::getObject(size_t index) const
		{
			return array.at(index);
		}

		vector<nullptr_t> JsonArrayWrapper::getAsNullArray() const
		{
			vector<nullptr_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<nullptr_t>(i.data.front().second));
			}

			return result;
		}

		vector<string> JsonArrayWrapper::getAsStringArray() const
		{
			vector<string> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<string>(i.data.front().second));
			}

			return result;
		}

		vector<bool> JsonArrayWrapper::getAsBoolArray() const
		{
			vector<bool> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<bool>(i.data.front().second));
			}

			return result;
		}

		vector<int64_t> JsonArrayWrapper::getAsInt64_tArray() const
		{
			vector<int64_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<int64_t>(i.data.front().second));
			}

			return result;
		}

		vector<uint64_t> JsonArrayWrapper::getAsUInt64_tArray() const
		{
			vector<uint64_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<uint64_t>(i.data.front().second));
			}

			return result;
		}

		vector<double> JsonArrayWrapper::getAsDoubleArray() const
		{
			vector<double> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<double>(i.data.front().second));
			}

			return result;
		}

		vector<JsonObject> JsonArrayWrapper::getAsObjectArray() const
		{
			vector<JsonObject> result;

			for (const auto& i : array)
			{
				const JsonObject& item = get<JsonObject>(i.data.front().second);

				result.push_back(item);
			}

			return result;
		}

		const string* JsonArrayWrapper::getOffset() const
		{
			return offset;
		}

		string* JsonArrayWrapper::getOffset()
		{
			return offset;
		}

		const vector<JsonObject>& JsonArrayWrapper::operator * () const
		{
			return array;
		}

		const JsonObject& JsonArrayWrapper::operator [] (size_t index) const
		{
			return array.at(index);
		}
	}
}
