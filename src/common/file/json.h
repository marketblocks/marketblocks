#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <format>

#include "json_iterator.h"
#include "common/types/proxy.h"
#include "common/exceptions/not_implemented_exception.h"

namespace cb
{
	typedef json<value_proxy<const nlohmann::json>> json_document;
	typedef json<reference_proxy<const nlohmann::json>> json_element;

	template<typename json_proxy>
	class json
	{
	private:
		friend json_iterator;

		json_proxy _proxy;

		template<typename Accessor>
		const json_element get_element(const Accessor& accessor) const
		{
			return json_element{ reference_proxy{ _proxy.value()[accessor] } };
		}

	public:
		explicit json(json_proxy&& proxy)
			: _proxy{ std::forward<json_proxy>(proxy) }
		{}

		template<typename T>
		const T get(const std::string& paramName) const
		{
			return _proxy.value()[paramName].get<T>();
		}

		template<typename T>
		const T get() const
		{
			return _proxy.value().get<T>();
		}

		const json_element element(const std::string& paramName) const
		{
			return get_element(paramName);
		}

		const json_element element(int index) const
		{
			return get_element(index);
		}

		size_t size() const
		{
			return _proxy.value().size();
		}

		std::string to_string() const
		{
			return _proxy.value().dump();
		}

		json_iterator begin() const
		{
			return json_iterator{ _proxy.value().begin() };
		}

		json_iterator end() const
		{
			return json_iterator{ _proxy.value().end() };
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