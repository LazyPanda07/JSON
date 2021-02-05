#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		class CantFindValueException : public BaseJSONException
		{
		public:
			CantFindValueException(const std::string& key);
		};
	}
}
