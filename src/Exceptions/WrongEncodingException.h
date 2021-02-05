#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		class WrongEncodingException : public BaseJSONException
		{
		public:
			WrongEncodingException();
		};
	}
}
