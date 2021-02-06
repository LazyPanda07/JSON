#include "JSONUtility.h"

#include "Exceptions/WrongEncodingException.h"

using namespace std;

namespace json
{
	namespace utility
	{
		string json::utility::toUTF8JSON(const string& source, unsigned int sourceCodepage)
		{
			string result;
			wstring tem;
			int size = MultiByteToWideChar
			(
				sourceCodepage,
				NULL,
				source.data(),
				-1,
				nullptr,
				NULL
			);

			if (!size)
			{
				throw json::exceptions::WrongEncodingException();
			}

			tem.resize(static_cast<size_t>(size) - 1);

			if (!MultiByteToWideChar
			(
				sourceCodepage,
				NULL,
				source.data(),
				-1,
				tem.data(),
				size
			))
			{
				throw json::exceptions::WrongEncodingException();
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
				throw json::exceptions::WrongEncodingException();
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
				throw json::exceptions::WrongEncodingException();
			}

			return result;
		}
	}
}
