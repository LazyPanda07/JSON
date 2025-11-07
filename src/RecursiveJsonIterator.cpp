//#include "RecursiveJsonIterator.h"
//
//namespace json
//{
//	RecursiveJsonIterator::RecursiveJsonIterator(const JsonParser& parser) :
//		object(parser.getParsedData())
//	{
//		depth.emplace(object.begin());
//	}
//
//	RecursiveJsonIterator::RecursiveJsonIterator(const JsonObject& object) :
//		object(object)
//	{
//		depth.emplace(this->object.begin());
//	}
//
//	RecursiveJsonIterator RecursiveJsonIterator::operator++ (int) noexcept
//	{
//		RecursiveJsonIterator it(*this);
//
//		++(*this);
//
//		return it;
//	}
//
//	const RecursiveJsonIterator& RecursiveJsonIterator::operator++ () noexcept
//	{
//		using utility::operator==;
//
//		if (depth.empty())
//		{
//			return *this;
//		}
//
//		JsonObject::ConstIterator& current = depth.top();
//
//		if (current == current.getEnd())
//		{
//			depth.pop();
//
//			return ++(*this);
//		}
//		else if (current->second.index() == utility::JsonVariantTypeEnum::jJSONObject)
//		{
//			const JsonObject& jsonObject = get<JsonObject>(current->second);
//
//			++current;
//
//			if (current == current.getEnd())
//			{
//				depth.pop();
//			}
//
//			depth.push(jsonObject.begin());
//
//			return *this;
//		}
//		else if (current->second.index() == utility::JsonVariantTypeEnum::jJSONArray)
//		{
//			const vector<JsonObject>& jsonArray = get<vector<JsonObject>>(current->second);
//
//			++current;
//
//			if (current == current.getEnd())
//			{
//				depth.pop();
//			}
//
//			for (auto it = jsonArray.rbegin(); it != jsonArray.rend(); ++it)
//			{
//				depth.push(it->begin());
//			}
//
//			return *this;
//		}
//
//		++current;
//
//		if (current == current.getEnd())
//		{
//			depth.pop();
//		}
//
//		return *this;
//	}
//
//	const JsonObject& RecursiveJsonIterator::operator* () const noexcept
//	{
//		return *depth.top();
//	}
//
//	const JsonObject* RecursiveJsonIterator::operator-> () const noexcept
//	{
//		return depth.top().operator->();
//	}
//
//	bool RecursiveJsonIterator::operator == (const RecursiveJsonIterator& other) const noexcept
//	{
//		if (depth.empty() && other.depth.empty())
//		{
//			return true;
//		}
//		else if (
//			depth.empty() && other.depth.size() ||
//			depth.size() && other.depth.empty()
//			)
//		{
//			return false;
//		}
//
//		return depth.top() == other.depth.top();
//	}
//
//	bool RecursiveJsonIterator::operator != (const RecursiveJsonIterator& other) const noexcept
//	{
//		return !(*this == other);
//	}
//}
