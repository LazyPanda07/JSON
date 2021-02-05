#include "WrongEncodingException.h"

namespace json
{
	namespace exceptions
	{
		WrongEncodingException::WrongEncodingException() :
			BaseJSONException("Can't convert to UTF8 encoding")
		{

		}
	}
}
