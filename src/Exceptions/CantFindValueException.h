#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Thrown if can't find value in JSONParser::get<jsonType>
		class JSON_API CantFindValueException : public BaseJSONException
		{
		public:
			CantFindValueException(std::string_view key);

			~CantFindValueException() = default;
		};
	}
}
