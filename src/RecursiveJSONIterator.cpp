#include "RecursiveJSONIterator.h"

using namespace std;

namespace json
{
	using ConstJSONIterator = utility::JSONObject::ConstJSONIterator;

	RecursiveJSONIterator::RecursiveJSONIterator(const JSONParser& parser) :
		object(parser.getParsedData())
	{
		depth.emplace(object.begin());
	}

	RecursiveJSONIterator::RecursiveJSONIterator(const utility::JSONObject& object) :
		object(object)
	{
		depth.emplace(this->object.begin());
	}

	RecursiveJSONIterator RecursiveJSONIterator::operator++ (int) noexcept
	{
		RecursiveJSONIterator it(*this);

		++(*this);

		return it;
	}

	const RecursiveJSONIterator& RecursiveJSONIterator::operator++ () noexcept
	{
		if (depth.empty())
		{
			return *this;
		}

		ConstJSONIterator& current = depth.top();

		if (current == current.getEnd())
		{
			depth.pop();

			return ++(*this);
		}
		else if (current->second.index() == utility::VariantTypeEnum::jJSONObject)
		{
			const utility::JSONObject& jsonObject = get<utility::JSONObject>(current->second);

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
			const vector<utility::JSONObject>& jsonArray = get<vector<utility::JSONObject>>(current->second);

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

	const pair<string, utility::JSONObject::VariantType>& RecursiveJSONIterator::operator* () const noexcept
	{
		return *depth.top();
	}

	utility::JSONObject::ConstJSONIterator::ConstJSONIteratorType RecursiveJSONIterator::operator-> () const noexcept
	{
		return depth.top();
	}

	bool RecursiveJSONIterator::operator == (const RecursiveJSONIterator& other) const noexcept
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

	bool RecursiveJSONIterator::operator != (const RecursiveJSONIterator& other) const noexcept
	{
		return !(*this == other);
	}
}
