#pragma once

#include "JSONUtility.h"

namespace json
{
	class JSONBuilder
	{
	private:
		utility::jsonBuilderStruct builderData;

	public:
		JSONBuilder() = default;

		~JSONBuilder() = default;
	};
}
