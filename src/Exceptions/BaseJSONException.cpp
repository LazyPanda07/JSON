#include "BaseJSONException.h"

using namespace std;

namespace json
{
	namespace exceptions
	{
		BaseJSONException::BaseJSONException(const string& errorMessage) :
			runtime_error(errorMessage)
		{

		}
	}
}
