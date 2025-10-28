#include "RecursiveIterator.h"

using namespace std;

namespace json
{
	using ConstIterator = JsonObject::ConstIterator;

	RecursiveIterator::RecursiveIterator(const JsonParser& parser) :
		object(parser.getParsedData())
	{
		depth.emplace(object.begin());
	}

	RecursiveIterator::RecursiveIterator(const JsonObject& object) :
		object(object)
	{
		depth.emplace(this->object.begin());
	}

	RecursiveIterator RecursiveIterator::operator++ (int) noexcept
	{
		RecursiveIterator it(*this);

		++(*this);

		return it;
	}

	const RecursiveIterator& RecursiveIterator::operator++ () noexcept
	{
		if (depth.empty())
		{
			return *this;
		}

		ConstIterator& current = depth.top();

		if (current == current.getEnd())
		{
			depth.pop();

			return ++(*this);
		}
		else if (current->second.index() == utility::VariantTypeEnum::jJSONObject)
		{
			const JsonObject& jsonObject = get<JsonObject>(current->second);

			++current;

			if (current == current.getEnd())
			{
				depth.pop();
			}

			depth.push(jsonObject.begin());

			return *this;
		}
		else if (current->second.index() == utility::VariantTypeEnum::jJSONArray)
		{
			const vector<JsonObject>& jsonArray = get<vector<JsonObject>>(current->second);

			++current;

			if (current == current.getEnd())
			{
				depth.pop();
			}

			for (auto it = jsonArray.rbegin(); it != jsonArray.rend(); ++it)
			{
				depth.push(it->begin());
			}

			return *this;
		}

		++current;

		if (current == current.getEnd())
		{
			depth.pop();
		}

		return *this;
	}

	const pair<string, JsonObject::VariantType>& RecursiveIterator::operator* () const noexcept
	{
		return *depth.top();
	}

	JsonObject::ConstIterator::ConstIteratorType RecursiveIterator::operator-> () const noexcept
	{
		return depth.top();
	}

	bool RecursiveIterator::operator == (const RecursiveIterator& other) const noexcept
	{
		if (depth.empty() && other.depth.empty())
		{
			return true;
		}
		else if (
			depth.empty() && other.depth.size() ||
			depth.size() && other.depth.empty()
			)
		{
			return false;
		}

		return depth.top() == other.depth.top();
	}

	bool RecursiveIterator::operator != (const RecursiveIterator& other) const noexcept
	{
		return !(*this == other);
	}
}
