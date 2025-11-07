//#pragma once
//
//#include <stack>
//
//#include "JsonParser.h"
//
//namespace json
//{
//	class RecursiveJsonIterator
//	{
//	private:
//		JsonObject object;
//		std::stack<JsonObject::ConstIterator> depth;
//
//	private:
//		RecursiveJsonIterator() = default;
//
//	public:
//		RecursiveJsonIterator(const JsonParser& parser);
//
//		RecursiveJsonIterator(const JsonObject& object);
//
//		RecursiveJsonIterator operator ++ (int) noexcept;
//
//		const RecursiveJsonIterator& operator ++ () noexcept;
//
//		const JsonObject& operator* () const noexcept;
//
//		const JsonObject* operator-> () const noexcept;
//
//		bool operator == (const RecursiveJsonIterator& other) const noexcept;
//
//		bool operator != (const RecursiveJsonIterator& other) const noexcept;
//
//		friend RecursiveJsonIterator end(const RecursiveJsonIterator& it) noexcept;
//
//		~RecursiveJsonIterator() = default;
//	};
//
//	inline RecursiveJsonIterator begin(const RecursiveJsonIterator& it) noexcept
//	{
//		return it;
//	}
//
//	inline RecursiveJsonIterator end(const RecursiveJsonIterator&) noexcept
//	{
//		return RecursiveJsonIterator();
//	}
//}
