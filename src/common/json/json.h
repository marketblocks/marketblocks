#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <format>

#include "json_constants.h"
#include "json_iterator.h"
#include "json_writer.h"

namespace cb
{
	template<typename json_object>
	class json
	{
	private:
		friend json_iterator;
		friend json_writer;

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
		T get(int index) const
		{
			return _json[index].get<T>();
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
	json_document to_json(const T& t)
	{
		json_writer writer;
		to_json<T>(t, writer);
		return writer.to_json();
	}
}