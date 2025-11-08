#include "Exceptions/CantFindValueException.h"

#include <format>

using namespace std;

namespace json
{
	namespace exceptions
	{
		CantFindValueException::CantFindValueException(string_view key) :
			BaseJSONException(format(R"(Can't find JSON value with key "{}")", key))
		{

		}
	}
}
