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

	enum class json_value_type
	{
		OBJECT,
		ARRAY,
		INT,
		DOUBLE,
		BOOL,
		STRING,
		UNKNOWN
	};

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
		T get(std::string_view paramName) const
		{
			return _json[paramName.data()].get<T>();
		}

		template<typename T>
		T get() const
		{
			return _json.get<T>();
		}

		const json_element element(std::string_view paramName) const
		{
			return json_element{ _json[paramName.data()]};
		}

		const json_element element(int index) const
		{
			return json_element{ _json[index] };
		}

		bool has_member(std::string_view paramName) const
		{
			return _json.contains(paramName.data());
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

		json_value_type type() const
		{
			nlohmann::detail::value_t type = _json.type();

			switch (type)
			{
			case nlohmann::detail::value_t::object:
				return json_value_type::OBJECT;
			case nlohmann::detail::value_t::array:
				return json_value_type::ARRAY;
			case nlohmann::detail::value_t::number_integer:
				return json_value_type::INT;
			case nlohmann::detail::value_t::number_float:
				return json_value_type::DOUBLE;
			case nlohmann::detail::value_t::boolean:
				return json_value_type::BOOL;
			case nlohmann::detail::value_t::string:
				return json_value_type::STRING;

			default:
				return json_value_type::UNKNOWN;
			}
		}
	};

	class json_writer
	{
	private:
		nlohmann::json _document;

	public:
		template<typename T>
		void add(std::string_view propertyName, T value)
		{
			_document[propertyName.data()] = std::move(value);
		}

		std::string to_string() const
		{
			return _document.dump();
		}
	};

	json_document parse_json(std::string_view jsonString);

	template<typename T>
	T from_json(const json_document& json)
	{
		static_assert(sizeof(T) == 0, "No specialization of from_json found");
	}

	template<typename T>
	T from_json(std::string_view json)
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