#pragma once

#include <string>
#include <unordered_map>

namespace cb
{
	enum class exchange_id
	{
		KRAKEN
	};

	class exchange_id_lookup
	{
	private:
		std::unordered_map<std::string, exchange_id> _container;

	public:
		exchange_id_lookup();

		const std::unordered_map<std::string, exchange_id>& map() const { return _container; }
		std::vector<std::string> all_ids() const;
	};
}
