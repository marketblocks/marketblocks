#pragma once

#include "trading/fee_schedule.h"
#include "common/types/unordered_string_map.h"
#include "common/json/json.h"

namespace mb
{
	class paper_trading_config
	{
	private:
		fee_schedule _fees;
		unordered_string_map<double> _balances;

	public:
		paper_trading_config();
		paper_trading_config(fee_schedule fees, unordered_string_map<double> balances);
		
		static constexpr std::string name() noexcept { return "paper_trading"; }

		const fee_schedule& fees() const noexcept { return _fees; }
		const unordered_string_map<double>& balances() const noexcept { return _balances; }
	};

	template<>
	paper_trading_config from_json<paper_trading_config>(const json_document& json);

	template<>
	void to_json<paper_trading_config>(const paper_trading_config& config, json_writer& writer);
}