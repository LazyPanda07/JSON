#pragma once

#include <stdexcept>

namespace json
{
	namespace exceptions
	{
		class BaseJSONException : public std::runtime_error
		{
		public:
			BaseJSONException(const std::string& errorMessage);
		};
	}
}
