#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Thrown if can't encode or decode to UTF8 or from UTF8
		class WrongEncodingException : public BaseJSONException
		{
		public:
			WrongEncodingException();

			~WrongEncodingException() = default;
		};
	}
}
