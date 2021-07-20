#include "JSONUtility.h"

#include <Windows.h>

#include "Exceptions/WrongEncodingException.h"

using namespace std;

namespace json
{
	namespace utility
	{
		string toUTF8JSON(const string& source, uint32_t sourceCodePage)
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

		string fromUTF8JSON(const string& source, uint32_t resultCodePage)
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

		void outputJSONType(ostream& outputStream, const jsonObject::variantType& value, bool isLast)
		{
			variantTypeEnum type = static_cast<variantTypeEnum>(value.index());

			if (type >= variantTypeEnum::jJSONArray)
			{
				jsonObject::offset += "  ";
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

			case variantTypeEnum::JUInt64_t:
				outputStream << get<uint64_t>(value);

				break;

			case variantTypeEnum::jDouble:
				outputStream << fixed << get<double>(value);

				break;

			case variantTypeEnum::jJSONArray:
				outputStream << value << string(jsonObject::offset.begin(), jsonObject::offset.end() - 2) << ']';

				break;

			case variantTypeEnum::jJSONObject:
			{
#ifdef JSON_DLL
				const shared_ptr<jsonObject>& ref = get<shared_ptr<jsonObject>>(value);
#else
				const unique_ptr<jsonObject>& ref = get<unique_ptr<jsonObject>>(value);
#endif // JSON_DLL

				auto start = ref->data.begin();
				auto end = ref->data.end();

				outputStream << "{\n";

				while (start != end)
				{
					auto check = start;

					if (start->first.size())
					{
						outputStream << jsonObject::offset << '"' << start->first << '"' << ": ";
					}
					else
					{
						outputStream << utility::jsonObject::offset;
					}

					outputJSONType(outputStream, start->second, ++check == end);

					++start;
				}

				outputStream << string(jsonObject::offset.begin(), jsonObject::offset.end() - 2) << '}';
			}

			break;
			}

			if (type >= utility::variantTypeEnum::jJSONArray)
			{
				jsonObject::offset.pop_back();
				jsonObject::offset.pop_back();
			}

			if (!isLast)
			{
				outputStream << ',';
			}

			outputStream << endl;
		}

		ostream& operator << (ostream& outputStream, const jsonObject::variantType& jsonData)
		{
			outputStream << "[\n";

			auto& jsonArray = get<static_cast<size_t>(variantTypeEnum::jJSONArray)>(jsonData);

			for (size_t i = 0; i < jsonArray.size(); i++)
			{
				for (const auto& j : jsonArray[i]->data)
				{
					outputStream << jsonObject::offset;

					outputJSONType(outputStream, j.second, i + 1 == jsonArray.size());
				}
			}

			return outputStream;
		}
	}
}
