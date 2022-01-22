#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "json_iterator.h"
#include "common/types/proxy.h"

namespace cb
{
	typedef json<value_proxy<nlohmann::json>> json_document;
	typedef json<reference_proxy<nlohmann::json>> json_element;

	template<typename json_proxy>
	class json
	{
	private:
		friend json_iterator;

		json_proxy _proxy;

	public:
		explicit json(json_proxy&& proxy)
			: _proxy{ std::forward<json_proxy>(proxy) }
		{}

		template<typename T>
		T get(const std::string& paramName)
		{
			return _proxy.value()[paramName].get<T>();
		}

		template<>
		json_element get<json_element> (const std::string& paramName)
		{
			return json_element{ reference_proxy(_proxy.value()[paramName])};
		}

		template<typename T>
		T get()
		{
			return _proxy.value().get<T>();
		}

		json_element element(int index)
		{
			return json_element{ reference_proxy(_proxy.value()[index])};
		}

		size_t size()
		{
			return _proxy.value().size();
		}

		json_iterator begin()
		{
			return json_iterator{ _proxy.value().begin() };
		}

		json_iterator end()
		{
			return json_iterator{ _proxy.value().end() };
		}
	};

	json_document parse_json(const std::string& jsonString);
}