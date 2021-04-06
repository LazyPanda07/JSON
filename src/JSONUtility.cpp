#include "JSONUtility.h"

#include <Windows.h>

#include "Exceptions/WrongEncodingException.h"

using namespace std;


string json::utility::toUTF8JSON(const string& source, unsigned int sourceCodePage)
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
		throw json::exceptions::WrongEncodingException();
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

string json::utility::fromUTF8JSON(const string& source, unsigned int resultCodePage)
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
		throw json::exceptions::WrongEncodingException();
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
		throw json::exceptions::WrongEncodingException();
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
		throw json::exceptions::WrongEncodingException();
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
		throw json::exceptions::WrongEncodingException();
	}

	return result;
}
