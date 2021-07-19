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

		void outputJSONType(std::ostream& outputStream, const jsonObject::variantType& value, bool isLast)
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
				outputStream << '"' << std::get<std::string>(value) << '"';

				break;

			case variantTypeEnum::jBool:
				outputStream << std::boolalpha << std::get<bool>(value);

				break;

			case variantTypeEnum::jInt64_t:
				outputStream << std::get<int64_t>(value);

				break;

			case variantTypeEnum::JUInt64_t:
				outputStream << std::get<uint64_t>(value);

				break;

			case variantTypeEnum::jDouble:
				outputStream << std::fixed << std::get<double>(value);

				break;

				// TODO: доделать
			case variantTypeEnum::jJSONArray:
				outputStream << value << std::string(jsonObject::offset.begin(), jsonObject::offset.end() - 2) << ']';

				break;

			case variantTypeEnum::jJSONObject:
			{
#ifdef JSON_DLL
				const std::shared_ptr<jsonObject>& ref = std::get<std::shared_ptr<jsonObject>>(value);
#else
				const std::unique_ptr<jsonObject>& ref = std::get<std::unique_ptr<jsonObject>>(value);
#endif // JSON_DLL

				auto start = ref->data.begin();
				auto end = ref->data.end();

				outputStream << "{\n";

				while (start != end)
				{
					auto check = start;

					outputStream << jsonObject::offset << '"' << start->first << '"' << ": ";

					outputJSONType(outputStream, start->second, ++check == end);

					++start;
				}

				outputStream << std::string(jsonObject::offset.begin(), jsonObject::offset.end() - 2) << '}';
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

			outputStream << std::endl;
		}

		ostream& operator << (ostream& outputStream, const jsonObject::variantType& jsonData)
		{
			outputStream << "[\n";

			auto& jsonArray = get<static_cast<size_t>(variantTypeEnum::jJSONArray)>(jsonData);

			for (size_t i = 0; i < jsonArray.size(); i++)
			{
				for (const auto& j : jsonArray[i]->data)
				{
					variantTypeEnum type = static_cast<variantTypeEnum>(j.second.index());
					bool isLast = i + 1 == jsonArray.size();

					outputStream << jsonObject::offset;

					switch (type)
					{
					case variantTypeEnum::jNull:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::jString:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::jBool:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::jInt64_t:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::JUInt64_t:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::jDouble:
						outputJSONType(outputStream, j.second, isLast);

						break;
					case variantTypeEnum::jJSONArray:
						outputStream << j.second;

						break;
					case variantTypeEnum::jJSONObject:
						

						break;
					}
				}
			}

			return outputStream;
		}
	}
}
