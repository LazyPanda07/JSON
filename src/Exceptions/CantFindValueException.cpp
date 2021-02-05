#include "CantFindValueException.h"

using namespace std;

namespace json
{
	namespace exceptions
	{
		CantFindValueException::CantFindValueException(const string& key) :
			BaseJSONException("Can't find JSON value with key \"" + key + '"')
		{

		}
	}
}
