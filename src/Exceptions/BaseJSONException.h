#pragma once

#include <stdexcept>

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
