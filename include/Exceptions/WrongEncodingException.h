#pragma once

#include "BaseJSONException.h"

namespace json::exceptions
{
	/// @brief Thrown if can't encode or decode to UTF8 or from UTF8
	class WrongEncodingException : public BaseJSONException
	{
	public:
		WrongEncodingException(std::string_view source);

		~WrongEncodingException() = default;
	};
}
