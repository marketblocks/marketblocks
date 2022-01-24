#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <format>

#include "json_iterator.h"

namespace cb
{
	typedef json<nlohmann::json> json_document;
	typedef json<const nlohmann::json&> json_element;

	template<typename json_object>
	class json
	{
	private:
		friend json_iterator;

		json_object _json;

	public:
		explicit json(json_object&& object)
			: _json( std::forward<json_object>(object) )
		{}

		template<typename T>
		T get(const std::string& paramName) const
		{
			return _json[paramName].get<T>();
		}

		template<typename T>
		T get() const
		{
			return _json.get<T>();
		}

		const json_element element(const std::string& paramName) const
		{
			return json_element{ _json[paramName] };
		}

		const json_element element(int index) const
		{
			return json_element{ _json[index] };
		}

		size_t size() const
		{
			return _json.size();
		}

		std::string to_string() const
		{
			return _json.dump();
		}

		json_iterator begin() const
		{
			return json_iterator{ _json.begin() };
		}

		json_iterator end() const
		{
			return json_iterator{ _json.end() };
		}
	};

	class json_writer
	{
	private:
		nlohmann::json _document;

	public:
		template<typename T>
		void add(const std::string& propertyName, T value)
		{
			_document[propertyName] = std::move(value);
		}

		std::string to_string() const
		{
			return _document.dump();
		}
	};

	json_document parse_json(const std::string& jsonString);

	template<typename T>
	T from_json(const json_document& json)
	{
		static_assert(sizeof(T) == 0, "No specialization of from_json found");
	}

	template<typename T>
	T from_json(const std::string& json)
	{
		return from_json<T>(parse_json(json));
	}

	template<typename T>
	void to_json(const T& t, json_writer& writer)
	{
		static_assert(sizeof(T) == 0, "No specialization of to_json found");
	}

	template<typename T>
	std::string to_json(const T& t)
	{
		json_writer writer;
		to_json<T>(t, writer);
		return writer.to_string();
	}
}