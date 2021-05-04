#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Thrown if can't find value in JSONParser::get<jsonType>
		class CantFindValueException : public BaseJSONException
		{
		public:
			CantFindValueException(const std::string& key);

			~CantFindValueException() = default;
		};
	}
}
