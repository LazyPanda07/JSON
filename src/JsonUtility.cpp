#include "JsonUtility.h"

#include <algorithm>
#include <functional>

#if defined(__LINUX__) || defined(__ANDROID__)
#include <iconv.h>
#else
#include <Windows.h>
#endif

#include "OutputOperations.h"
#include "JsonObject.h"
#include "Exceptions/WrongEncodingException.h"
#include "Exceptions/CantFindValueException.h"

namespace json::utility
{
#if defined(__LINUX__) || defined(__ANDROID__)
	static std::string convertString(std::string_view source, size_t resultSize, iconv_t convert)
	{
		if (convert == reinterpret_cast<iconv_t>(std::string::npos))
		{
			throw exceptions::WrongEncodingException(source);
		}

		char* data = const_cast<char*>(source.data());
		size_t dataSize = source.size();
		std::string result(resultSize, '\0');
		char* resultData = const_cast<char*>(result.data());

		size_t error = iconv(convert, &data, &dataSize, &resultData, &resultSize);

		iconv_close(convert);

		if (error == std::string::npos)
		{
			throw exceptions::WrongEncodingException(source);
		}

		result.resize(result.size() - resultSize);

		return result;
	}

	std::string toUTF8JSON(std::string_view source, std::string_view sourceCodePage)
	{
		return convertString(source, source.size() * 4, iconv_open(CP_UTF8, sourceCodePage.data()));
	}

	std::string fromUTF8JSON(std::string_view source, std::string_view resultCodePage)
	{
		return convertString(source, source.size(), iconv_open(resultCodePage.data(), CP_UTF8));
	}

	std::string convertEncoding(std::string_view source, std::string_view sourceCodePage, std::string_view resultCodePage)
	{
		return convertString(source, source.size() * 4, iconv_open(resultCodePage.data(), sourceCodePage.data()));
	}
#else
	std::string toUTF8JSON(std::string_view source, uint32_t sourceCodePage)
	{
		std::string result;
		std::wstring tem;
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
		}

		return result;
	}

	std::string fromUTF8JSON(std::string_view source, uint32_t resultCodePage)
	{
		std::string result;
		std::wstring tem;
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
		}

		return result;
	}

	std::string convertEncoding(std::string_view source, uint32_t sourceCodePage, uint32_t resultCodePage)
	{
		std::string result;
		std::wstring tem;
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
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
			throw exceptions::WrongEncodingException(source);
		}

		return result;
	}
#endif

	std::string getJSONVersion()
	{
		std::string jsonVersion = "3.2.0";

		return jsonVersion;
	}
}
