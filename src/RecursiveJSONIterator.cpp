#include "RecursiveJSONIterator.h"

using namespace std;

namespace json
{
	using ConstJSONIterator = utility::jsonObject::ConstJSONIterator;

	RecursiveJSONIterator::RecursiveJSONIterator(const JSONParser& parser)
	{
		depth.emplace(parser.begin());
	}

	RecursiveJSONIterator::RecursiveJSONIterator(const utility::jsonObject& object)
	{
		depth.emplace(object.begin());
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

		++current;

		if (current == current.getEnd())
		{
			depth.pop();

			return ++(*this);
		}

		return *this;
	}

	const pair<string, utility::jsonObject::variantType>& RecursiveJSONIterator::operator* () const noexcept
	{
		return *depth.top();
	}

	utility::jsonObject::ConstJSONIterator::ConstJSONIteratorType RecursiveJSONIterator::operator-> () const noexcept
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
