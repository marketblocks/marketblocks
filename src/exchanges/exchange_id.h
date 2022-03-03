#pragma once

#include <string>

#include "common/types/unordered_string_map.h"

namespace cb
{
	enum class exchange_id
	{
		KRAKEN
	};

	class exchange_id_lookup
	{
	private:
		unordered_string_map<exchange_id> _container;

	public:
		exchange_id_lookup();

		const unordered_string_map<exchange_id>& map() const noexcept { return _container; }
		std::vector<std::string> all_ids() const;
	};
}
