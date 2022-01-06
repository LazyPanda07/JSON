#pragma once

#include "BaseJSONException.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Thrown in JSONParser constructor if wrong input stream was passed
		class JSON_API WrongInputStreamException : public BaseJSONException
		{
		private:
			static std::string getErrorMessage(const std::istream& stream);

		public:
			WrongInputStreamException(const std::istream& stream);

			~WrongInputStreamException() = default;
		};
	}
}
