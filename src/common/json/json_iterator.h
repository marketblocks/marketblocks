#pragma once

#include <nlohmann/json.hpp>

#include "json_constants.h"

namespace cb
{
	class json_iterator
	{
	private:
		using iterator = nlohmann::detail::iter_impl<const nlohmann::json>;
		iterator _iter;

	public:
		explicit json_iterator(iterator iter);

		json_iterator operator++();
		bool operator!=(const json_iterator& other) const;
		const std::string& key() const;
		const json_element value() const;
	};
}