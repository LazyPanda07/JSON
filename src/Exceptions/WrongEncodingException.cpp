#include "WrongEncodingException.h"

#include <format>

namespace json::exceptions
{
	WrongEncodingException::WrongEncodingException(std::string_view source) :
		BaseJSONException(std::format("Can't convert {} to another encoding", source))
	{

	}
}
