#include "JSONArrayWrapper.h"

using namespace std;

namespace json
{
	namespace utility
	{
		JSONArrayWrapper::JSONArrayWrapper(const vector<objectSmartPointer<jsonObject>>& array, string* offset) :
			array(array),
			offset(offset)
		{

		}

		JSONArrayWrapper::JSONArrayWrapper(const jsonObject::variantType& array, string* offset) :
			array(get<vector<objectSmartPointer<jsonObject>>>(array)),
			offset(offset)
		{

		}

		size_t JSONArrayWrapper::size() const
		{
			return array.size();
		}

		nullptr_t JSONArrayWrapper::getNull(size_t index) const
		{
			return get<nullptr_t>(array.at(index)->data.front().second);
		}

		string JSONArrayWrapper::getString(size_t index) const
		{
			return get<string>(array.at(index)->data.front().second);
		}

		bool JSONArrayWrapper::getBool(size_t index) const
		{
			return get<bool>(array.at(index)->data.front().second);
		}

		int64_t JSONArrayWrapper::getInt64_t(size_t index) const
		{
			return get<int64_t>(array.at(index)->data.front().second);
		}

		uint64_t JSONArrayWrapper::getUInt64_t(size_t index) const
		{
			return get<uint64_t>(array.at(index)->data.front().second);
		}

		double JSONArrayWrapper::getDouble(size_t index) const
		{
			return get<double>(array.at(index)->data.front().second);
		}

		const objectSmartPointer<jsonObject>& JSONArrayWrapper::getObject(size_t index) const
		{
			return array.at(index);
		}

		vector<nullptr_t> JSONArrayWrapper::getAsNullArray() const
		{
			vector<nullptr_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<nullptr_t>(i->data.front().second));
			}

			return result;
		}

		vector<string> JSONArrayWrapper::getAsStringArray() const
		{
			vector<string> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<string>(i->data.front().second));
			}

			return result;
		}

		vector<bool> JSONArrayWrapper::getAsBoolArray() const
		{
			vector<bool> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<bool>(i->data.front().second));
			}

			return result;
		}

		vector<int64_t> JSONArrayWrapper::getAsInt64_tArray() const
		{
			vector<int64_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<int64_t>(i->data.front().second));
			}

			return result;
		}

		vector<uint64_t> JSONArrayWrapper::getAsUInt64_tArray() const
		{
			vector<uint64_t> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<uint64_t>(i->data.front().second));
			}

			return result;
		}

		vector<double> JSONArrayWrapper::getAsDoubleArray() const
		{
			vector<double> result;

			result.reserve(array.size());

			for (const auto& i : array)
			{
				result.push_back(get<double>(i->data.front().second));
			}

			return result;
		}

		const string* JSONArrayWrapper::getOffset() const
		{
			return offset;
		}

		string* JSONArrayWrapper::getOffset()
		{
			return offset;
		}

		const vector<objectSmartPointer<jsonObject>>& JSONArrayWrapper::operator * () const
		{
			return array;
		}

		const objectSmartPointer<jsonObject>& JSONArrayWrapper::operator [] (size_t index) const
		{
			return array.at(index);
		}
	}
}
