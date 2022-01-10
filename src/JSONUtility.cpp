#include "JSONUtility.h"

#include <algorithm>
#include <functional>

#include <Windows.h>

#include "JSONArrayWrapper.h"

#include "Exceptions/WrongEncodingException.h"
#include "Exceptions/CantFindValueException.h"

using namespace std;

namespace json
{
	namespace utility
	{
		jsonObject::jsonObject(const jsonObject& other)
		{
			(*this) = other;
		}

		jsonObject& jsonObject::operator = (const jsonObject& other)
		{
			function<void(const string&, const variantType&, vector<pair<string, variantType>>&)> appendData = [&appendData](const string& key, const variantType& value, vector<pair<string, variantType>>& data)
			{
				switch (static_cast<variantTypeEnum>(value.index()))
				{
				case json::utility::variantTypeEnum::jNull:
					data.push_back({ key, get<nullptr_t>(value) });

					break;

				case json::utility::variantTypeEnum::jString:
					data.push_back({ key, get<string>(value) });

					break;

				case json::utility::variantTypeEnum::jBool:
					data.push_back({ key, get<bool>(value) });

					break;

				case json::utility::variantTypeEnum::jInt64_t:
					data.push_back({ key, get<int64_t>(value) });

					break;

				case json::utility::variantTypeEnum::jUInt64_t:
					data.push_back({ key, get<uint64_t>(value) });

					break;

				case json::utility::variantTypeEnum::jDouble:
					data.push_back({ key, get<double>(value) });

					break;

				case json::utility::variantTypeEnum::jJSONArray:
				{
					const vector<objectSmartPointer<jsonObject>>& currentArray = get<vector<objectSmartPointer<jsonObject>>>(value);
					vector<objectSmartPointer<jsonObject>> tem;

					for (const auto& i : currentArray)
					{
						objectSmartPointer<jsonObject> object = make_object<jsonObject>();

						appendData("", i->data.front().second, object->data);

						appendArray(move(object->data.front().second), tem);
					}

					data.push_back({ key, move(tem) });
				}

				break;

				case json::utility::variantTypeEnum::jJSONObject:
				{
					objectSmartPointer<jsonObject> tem = make_object<jsonObject>(*get<objectSmartPointer<jsonObject>>(value));

					data.push_back({ key, move(tem) });
				}

				break;

				default:
					break;
				}
			};

			data.clear();

			for (const auto& [key, value] : other.data)
			{
				appendData(key, value, data);
			}

			return *this;
		}

		nullptr_t jsonObject::getNull(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			return get<nullptr_t>(it->second);
		}

		const string& jsonObject::getString(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			return get<string>(it->second);
		}

		bool jsonObject::getBool(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			return get<bool>(it->second);
		}

		int64_t jsonObject::getInt(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}
			
			variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

			switch (type)
			{
			case json::utility::variantTypeEnum::jUInt64_t:
				return static_cast<int64_t>(get<uint64_t>(it->second));

			case json::utility::variantTypeEnum::jDouble:
				return static_cast<int64_t>(get<double>(it->second));
			}

			return get<int64_t>(it->second);
		}

		uint64_t jsonObject::getUnsignedInt(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

			switch (type)
			{
			case json::utility::variantTypeEnum::jInt64_t:
				return static_cast<uint64_t>(get<int64_t>(it->second));

			case json::utility::variantTypeEnum::jDouble:
				return static_cast<uint64_t>(get<double>(it->second));
			}

			return get<uint64_t>(it->second);
		}

		double jsonObject::getDouble(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			variantTypeEnum type = static_cast<variantTypeEnum>(it->second.index());

			switch (type)
			{
			case json::utility::variantTypeEnum::jInt64_t:
				return static_cast<double>(get<int64_t>(it->second));

			case json::utility::variantTypeEnum::jUInt64_t:
				return static_cast<double>(get<uint64_t>(it->second));
			}

			return get<double>(it->second);
		}

		const vector<objectSmartPointer<jsonObject>>& jsonObject::getArray(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			return get<vector<objectSmartPointer<jsonObject>>>(it->second);
		}

		const objectSmartPointer<jsonObject>& jsonObject::getObject(const string& key) const
		{
			auto it = find_if(data.begin(), data.end(), [&key](const pair<string, variantType>& value) { return value.first == key; });

			if (it == data.end())
			{
				throw exceptions::CantFindValueException(key);
			}

			return get<objectSmartPointer<jsonObject>>( it->second);
		}

		bool jsonObject::contains(const string& key, utility::variantTypeEnum type) const
		{
			return any_of(data.begin(), data.end(), [&key, &type](const pair<string, jsonObject::variantType>& data) { return data.first == key && data.second.index() == static_cast<size_t>(type); });
		}

		JSON_API_FUNCTION string toUTF8JSON(const string& source, uint32_t sourceCodePage)
		{
			string result;
			wstring tem;
			int size = MultiByteToWideChar
			(
				sourceCodePage,
				NULL,
				source.data(),
				-1,
				nullptr,
				NULL
			);

			if (!size)
			{
				throw exceptions::WrongEncodingException();
			}

			tem.resize(static_cast<size_t>(size) - 1);

			if (!MultiByteToWideChar
			(
				sourceCodePage,
				NULL,
				source.data(),
				-1,
				tem.data(),
				size
			))
			{
				throw exceptions::WrongEncodingException();
			}

			size = WideCharToMultiByte
			(
				CP_UTF8,
				NULL,
				tem.data(),
				-1,
				nullptr,
				NULL,
				NULL,
				NULL
			);

			if (!size)
			{
				throw exceptions::WrongEncodingException();
			}

			result.resize(static_cast<size_t>(size) - 1);

			if (!WideCharToMultiByte
			(
				CP_UTF8,
				NULL,
				tem.data(),
				-1,
				result.data(),
				size,
				NULL,
				NULL
			))
			{
				throw exceptions::WrongEncodingException();
			}

			return result;
		}

		JSON_API_FUNCTION string fromUTF8JSON(const string& source, uint32_t resultCodePage)
		{
			string result;
			wstring tem;
			int size = MultiByteToWideChar
			(
				CP_UTF8,
				NULL,
				source.data(),
				-1,
				nullptr,
				NULL
			);

			if (!size)
			{
				throw exceptions::WrongEncodingException();
			}

			tem.resize(static_cast<size_t>(size) - 1);

			if (!MultiByteToWideChar
			(
				CP_UTF8,
				NULL,
				source.data(),
				-1,
				tem.data(),
				size
			))
			{
				throw exceptions::WrongEncodingException();
			}

			size = WideCharToMultiByte
			(
				resultCodePage,
				NULL,
				tem.data(),
				-1,
				nullptr,
				NULL,
				NULL,
				NULL
			);

			if (!size)
			{
				throw exceptions::WrongEncodingException();
			}

			result.resize(static_cast<size_t>(size) - 1);

			if (!WideCharToMultiByte
			(
				resultCodePage,
				NULL,
				tem.data(),
				-1,
				result.data(),
				size,
				NULL,
				NULL
			))
			{
				throw exceptions::WrongEncodingException();
			}

			return result;
		}

		JSON_API_FUNCTION void outputJSONType(ostream& outputStream, const jsonObject::variantType& value, bool isLast, string& offset)
		{
			variantTypeEnum type = static_cast<variantTypeEnum>(value.index());

			if (type >= variantTypeEnum::jJSONArray)
			{
				offset += "  ";
			}

			switch (type)
			{
			case variantTypeEnum::jNull:
				outputStream << "null";

				break;

			case variantTypeEnum::jString:
				outputStream << '"' << get<string>(value) << '"';

				break;

			case variantTypeEnum::jBool:
				outputStream << boolalpha << get<bool>(value);

				break;

			case variantTypeEnum::jInt64_t:
				outputStream << get<int64_t>(value);

				break;

			case variantTypeEnum::jUInt64_t:
				outputStream << get<uint64_t>(value);

				break;

			case variantTypeEnum::jDouble:
				outputStream << fixed << get<double>(value);

				break;

			case variantTypeEnum::jJSONArray:
				outputStream << JSONArrayWrapper(value, &offset) << string(offset.begin(), offset.end() - 2) << ']';

				break;

			case variantTypeEnum::jJSONObject:
			{
				const objectSmartPointer<jsonObject>& ref = get<objectSmartPointer<jsonObject>>(value);

				auto start = ref->data.begin();
				auto end = ref->data.end();

				outputStream << "{\n";

				while (start != end)
				{
					auto check = start;

					if (start->first.size())
					{
						outputStream << offset << '"' << start->first << '"' << ": ";
					}
					else
					{
						outputStream << offset;
					}

					outputJSONType(outputStream, start->second, ++check == end, offset);

					++start;
				}

				outputStream << string(offset.begin(), offset.end() - 2) << '}';
			}

			break;
			}

			if (type >= variantTypeEnum::jJSONArray)
			{
				offset.pop_back();
				offset.pop_back();
			}

			if (!isLast)
			{
				outputStream << ',';
			}

			outputStream << endl;
		}

		JSON_API_FUNCTION ostream& operator << (ostream& outputStream, JSONArrayWrapper jsonData)
		{
			outputStream << '[' << endl;

			auto& jsonArray = *jsonData;

			if (!jsonData.getOffset())
			{
				throw runtime_error("JSONArrayWrapper offset was nullptr");
			}

			string& offset = *jsonData.getOffset();

			for (size_t i = 0; i < jsonArray.size(); i++)
			{
				for (const auto& j : jsonArray[i]->data)
				{
					outputStream << offset;

					outputJSONType(outputStream, j.second, i + 1 == jsonArray.size(), offset);
				}
			}

			return outputStream;
		}

		JSON_API_FUNCTION void appendArray(jsonObject::variantType&& value, vector<objectSmartPointer<jsonObject>>& jsonArray)
		{
			objectSmartPointer<jsonObject> object = make_object<jsonObject>();

			object->data.push_back(make_pair(""s, move(value)));

			jsonArray.push_back(move(object));
		}

		JSON_API_FUNCTION string getJSONVersion()
		{
			return "1.8.3"s;
		}
	}
}
