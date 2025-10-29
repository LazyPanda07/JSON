#pragma once

#include <stdexcept>

#include "JsonUtility.h"

namespace json
{
	namespace exceptions
	{
		/// @brief Base class for JSON exceptions
		class BaseJSONException : public std::runtime_error
		{
		public:
			BaseJSONException(const std::string& errorMessage);

			virtual ~BaseJSONException() = default;
		};
	}
}
