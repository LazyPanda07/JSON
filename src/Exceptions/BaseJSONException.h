#pragma once

#include <stdexcept>

#include "JSONUtility.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Base class for JSON exceptions
		class JSON_API BaseJSONException : public std::runtime_error
		{
		public:
			BaseJSONException(const std::string& errorMessage);

			virtual ~BaseJSONException() = default;
		};
	}
}
