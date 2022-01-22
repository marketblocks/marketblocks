#pragma once

#include <nlohmann/json.hpp>
#include "common/types/proxy.h"

namespace cb
{
	template <typename proxy>
	class json;

	typedef json<reference_proxy<const nlohmann::json>> json_element;

	class json_iterator
	{
	private:
		using iterator = nlohmann::detail::iter_impl<const nlohmann::json>;
		iterator _iter;

	public:
		json_iterator(iterator iter);

		json_iterator operator++();
		bool operator!=(const json_iterator& other) const;
		const std::string& key() const;
		const json_element value() const;
	};
}