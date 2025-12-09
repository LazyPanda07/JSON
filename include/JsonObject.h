#pragma once

#include <format>
#include <queue>
#include <optional>

#include "JsonUtility.h"

namespace json
{
	/// @brief JSON object
	class JsonObject
	{
	public:
		using VariantType = utility::JsonVariantType<JsonObject>;
		using MapType = utility::JsonMapType<JsonObject>;

	public:
		class Iterator
		{
		private:
			using IteratorType = std::variant
				<
				std::vector<JsonObject>::iterator,
				MapType::iterator,
				std::pair<JsonObject*, size_t>
				>;

		private:
			IteratorType begin;
			IteratorType end;
			IteratorType current;

		public:
			Iterator() = default;

			Iterator(IteratorType begin, IteratorType end, IteratorType current);

			Iterator(const Iterator& other) = default;

			Iterator(Iterator&& other) noexcept = default;

			std::optional<std::string_view> key() const;

			Iterator& operator =(const Iterator& other) = default;

			Iterator& operator =(Iterator&& other) noexcept = default;

			Iterator operator ++(int) noexcept;

			Iterator& operator ++() noexcept;

			JsonObject& operator *() noexcept;

			JsonObject* operator ->() noexcept;

			bool operator ==(const Iterator& other) const noexcept;

			bool operator !=(const Iterator& other) const noexcept;

			~Iterator() = default;
		};

		class ConstIterator
		{
		private:
			using IteratorType = std::variant
				<
				std::vector<JsonObject>::const_iterator,
				MapType::const_iterator,
				std::pair<const JsonObject*, size_t>
				>;

		private:
			IteratorType begin;
			IteratorType end;
			IteratorType current;

		public:
			ConstIterator() = default;

			ConstIterator(IteratorType begin, IteratorType end, IteratorType current);

			ConstIterator(const ConstIterator& other) = default;

			ConstIterator(ConstIterator&& other) noexcept = default;

			std::optional<std::string_view> key() const;

			ConstIterator& operator =(const ConstIterator& other) = default;

			ConstIterator& operator =(ConstIterator&& other) noexcept = default;

			ConstIterator operator ++(int) noexcept;

			const ConstIterator& operator ++() noexcept;

			const JsonObject& operator *() const noexcept;

			const JsonObject* operator ->() const noexcept;

			bool operator ==(const ConstIterator& other) const noexcept;

			bool operator !=(const ConstIterator& other) const noexcept;

			~ConstIterator() = default;
		};

	private:
		static bool compareMaps(const MapType& first, const MapType& second);

	private:
		template<utility::JsonValues<JsonObject> T>
		JsonObject* find(std::string_view key, bool recursive);

		template<utility::JsonValues<JsonObject> T>
		const JsonObject* find(std::string_view key, bool recursive) const;

	public:
		template<utility::JsonVariantTypeEnum T>
		static JsonObject createDefaultJsonObject();

		template<typename... Args>
		static std::vector<json::JsonObject> makeArray(Args&&... args);

	private:
		VariantType data;

	public:
		JsonObject();

		JsonObject(VariantType&& data);

		/**
		 * @brief Returns the number of elements in the container.
		 * @return If JsonObject is map or array it returns actual size, std::numeric_limits<size_t>::max() otherwise.
		 */
		size_t size() const;

		JsonObject& at(size_t index);

		JsonObject& at(std::string_view key);

		const JsonObject& at(size_t index) const;

		const JsonObject& at(std::string_view key) const;

		Iterator begin() noexcept;

		Iterator end() noexcept;

		ConstIterator begin() const noexcept;

		ConstIterator end() const noexcept;

		const std::type_info& getType() const;

		utility::JsonVariantTypeEnum getEnumType() const;

		bool operator ==(const JsonObject& other) const noexcept;

		const JsonObject& operator [](size_t index) const;

		const JsonObject& operator [](std::string_view key) const;

		JsonObject& operator [](size_t index);

		template<typename T>
		JsonObject& emplace_back(T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>);

		template<utility::JsonValues<JsonObject> T>
		bool is() const;

		template<utility::JsonVariantTypeEnum T>
		bool is() const;

		template<utility::JsonValues<JsonObject> T>
		bool contains(std::string_view key, bool recursive = false) const;

		template<utility::JsonLightValues T>
		T get() const;

		template<utility::JsonHeavyValues<JsonObject> T>
		const T& get() const;

		template<utility::JsonValues<JsonObject> T>
		bool tryGet(std::string_view key, T& value, bool recursive = false) const;

		template<utility::JsonValues<JsonObject> T>
		bool tryGet(T& value) const;

		template<typename T>
		JsonObject& operator [](T&& key) requires(std::convertible_to<T, std::string_view> || (std::same_as<T, std::string> && std::is_rvalue_reference_v<decltype(key)>));

		template<typename T>
		JsonObject& operator =(T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>);

		friend std::ostream& operator <<(std::ostream& stream, const JsonObject& object);

		~JsonObject() = default;
	};
}

namespace json
{
	template<utility::JsonValues<JsonObject> T>
	JsonObject* JsonObject::find(std::string_view key, bool recursive)
	{
		std::queue<JsonObject*> objects;

		objects.push(this);

		while (objects.size())
		{
			JsonObject* current = objects.front();
			bool isObject = current->is<JsonObject>();

			objects.pop();

			for (auto it = current->begin(); it != current->end(); ++it)
			{
				JsonObject& value = *it;

				if (isObject)
				{
					if (it.key() == key && it->is<T>())
					{
						return &value;
					}
				}

				if (recursive && value.is<JsonObject>() || value.is<std::vector<JsonObject>>())
				{
					objects.push(&value);
				}
			}
		}

		return nullptr;
	}

	template<utility::JsonValues<JsonObject> T>
	const JsonObject* JsonObject::find(std::string_view key, bool recursive) const
	{
		std::queue<const JsonObject*> objects;

		objects.push(this);

		while (objects.size())
		{
			const JsonObject* current = objects.front();
			bool isObject = current->is<JsonObject>();

			objects.pop();

			for (auto it = current->begin(); it != current->end(); ++it)
			{
				const JsonObject& value = *it;

				if (isObject)
				{
					if (it.key() == key && it->is<T>())
					{
						return &value;
					}
				}

				if (recursive && value.is<JsonObject>() || value.is<std::vector<JsonObject>>())
				{
					objects.push(&value);
				}
			}
		}

		return nullptr;
	}

	template<utility::JsonVariantTypeEnum T>
	JsonObject JsonObject::createDefaultJsonObject()
	{
		switch (T)
		{
		case json::utility::JsonVariantTypeEnum::jNull:
			return JsonObject();

		case json::utility::JsonVariantTypeEnum::jString:
			return JsonObject("");

		case json::utility::JsonVariantTypeEnum::jBool:
			return JsonObject(false);

		case json::utility::JsonVariantTypeEnum::jInt:
			return JsonObject(static_cast<int64_t>(0));

		case json::utility::JsonVariantTypeEnum::jUint:
			return JsonObject(static_cast<uint64_t>(0));

		case json::utility::JsonVariantTypeEnum::jDouble:
			return JsonObject(0.0);

		case json::utility::JsonVariantTypeEnum::jJSONArray:
			return JsonObject(std::vector<JsonObject>());

		case json::utility::JsonVariantTypeEnum::jJSONObject:
			return JsonObject(JsonObject::MapType());

		default:
			throw std::runtime_error("Wrong JsonVariantTypeEnum");
		}
	}

	template<typename... Args>
	std::vector<json::JsonObject> JsonObject::makeArray(Args&&... args)
	{
		std::vector<json::JsonObject> result;
		auto helper = [&result](auto&& value)
			{
				result.emplace_back() = std::forward<decltype(value)>(value);
			};

		result.reserve(sizeof...(Args));

		(helper(std::forward<Args>(args)), ...);

		return result;
	}

	template<typename T>
	JsonObject& JsonObject::emplace_back(T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>)
	{
		using ActualT = std::remove_cvref_t<T>;

		if (!std::holds_alternative<std::vector<JsonObject>>(data))
		{
			data = std::vector<JsonObject>();
		}

		std::vector<JsonObject>& array = std::get<std::vector<JsonObject>>(data);

		if constexpr (std::same_as<ActualT, std::nullptr_t>)
		{
			array.emplace_back(nullptr);
		}
		else if constexpr (std::same_as<ActualT, std::string>)
		{
			array.emplace_back(std::forward<T>(value));
		}
		else if constexpr (std::convertible_to<ActualT, std::string_view>)
		{
			array.emplace_back(std::string(static_cast<std::string_view>(value)));
		}
		else if constexpr (std::convertible_to<ActualT, std::string>)
		{
			array.emplace_back(static_cast<std::string>(value));
		}
		else
		{
			array.emplace_back(std::forward<T>(value));
		}

		return array.back();
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::is() const
	{
		if constexpr (std::is_integral_v<T>)
		{
			return std::holds_alternative<bool>(data) || std::holds_alternative<int64_t>(data) || std::holds_alternative<uint64_t>(data);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return std::holds_alternative<double>(data);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			return std::holds_alternative<MapType>(data);
		}
		else
		{
			return std::holds_alternative<T>(data);
		}
	}

	template<utility::JsonVariantTypeEnum T>
	bool JsonObject::is() const
	{
		return data.index() == static_cast<size_t>(T);
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::contains(std::string_view key, bool recursive) const
	{
		return static_cast<bool>(this->find<T>(key, recursive));
	}

	template<utility::JsonLightValues T>
	T JsonObject::get() const
	{
		if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			return std::get<std::nullptr_t>(data);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return std::get<bool>(data);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return static_cast<T>(std::get<double>(data));
		}
		else if constexpr (std::is_integral_v<T>)
		{
			if (std::holds_alternative<int64_t>(data))
			{
				return static_cast<T>(std::get<int64_t>(data));
			}
			else
			{
				return static_cast<T>(std::get<uint64_t>(data));
			}
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonHeavyValues<JsonObject> T>
	const T& JsonObject::get() const
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			return std::get<std::string>(data);
		}
		else if constexpr (std::is_same_v<T, std::vector<JsonObject>>)
		{
			return std::get<std::vector<JsonObject>>(data);
		}
		else if constexpr (std::is_same_v<T, JsonObject>)
		{
			return *this;
		}
		else
		{
			throw std::runtime_error("Wrong type");
		}

		return {};
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::tryGet(std::string_view key, T& value, bool recursive) const
	{
		if (const JsonObject* result = this->find<T>(key, recursive))
		{
			value = result->get<T>();

			return true;
		}

		return false;
	}

	template<utility::JsonValues<JsonObject> T>
	bool JsonObject::tryGet(T& value) const
	{
		if (this->is<T>())
		{
			value = this->get<T>();

			return true;
		}

		return false;
	}

	template<typename T>
	JsonObject& JsonObject::operator =(T&& value) requires (utility::JsonValues<T, JsonObject> || std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>)
	{
		using ActualT = std::remove_cvref_t<T>;

		if constexpr (std::is_same_v<ActualT, std::nullptr_t> || std::is_same_v<ActualT, bool>)
		{
			data = value;
		}
		else if constexpr (std::is_same_v<ActualT, std::string>)
		{
			data = std::forward<T>(value);
		}
		else if constexpr (std::convertible_to<ActualT, std::string_view>)
		{
			data = static_cast<std::string_view>(value).data();
		}
		else if constexpr (std::is_convertible_v<ActualT, std::string>)
		{
			data = static_cast<std::string>(value);
		}
		else if constexpr (std::is_same_v<ActualT, std::vector<JsonObject>>)
		{
			data = std::forward<T>(value);
		}
		else if constexpr (std::is_same_v<ActualT, JsonObject>)
		{
			if constexpr (std::is_rvalue_reference_v<T>)
			{
				data = std::move(value.data);
			}
			else
			{
				data = value.data;
			}
		}
		else if constexpr (std::is_floating_point_v<ActualT>)
		{
			data = static_cast<double>(value);
		}
		else if constexpr (std::is_unsigned_v<ActualT>)
		{
			data = static_cast<uint64_t>(value);
		}
		else if constexpr (std::is_signed_v<ActualT>)
		{
			data = static_cast<int64_t>(value);
		}
		else
		{
			throw std::invalid_argument(std::format("Wrong argument type: {}", typeid(T).name()));
		}

		return *this;
	}

	template<typename T>
	JsonObject& JsonObject::operator [](T&& key) requires(std::convertible_to<T, std::string_view> || (std::same_as<T, std::string> && std::is_rvalue_reference_v<decltype(key)>))
	{
		MapType* map;

		if (!std::holds_alternative<MapType>(data))
		{
			data = MapType();
		}

		map = &std::get<MapType>(data);

		if (auto it = map->find(static_cast<std::string_view>(key)); it != map->end())
		{
			return it->second;
		}

		if constexpr (std::same_as<T, std::string>)
		{
			return map->emplace(std::move(key), JsonObject()).first->second;
		}
		else
		{
			return map->emplace(static_cast<std::string_view>(key), JsonObject()).first->second;
		}
	}
}
