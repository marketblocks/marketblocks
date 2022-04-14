#pragma once

#include "trading/fee_schedule.h"
#include "common/types/unordered_string_map.h"
#include "common/json/json.h"

namespace mb
{
	class paper_trading_config
	{
	private:
		double _fee;
		unordered_string_map<double> _balances;

	public:
		paper_trading_config();
		paper_trading_config(double fee, unordered_string_map<double> balances);
		
		static constexpr std::string name() noexcept { return "paper_trading"; }

		double fee() const noexcept { return _fee; }
		const unordered_string_map<double>& balances() const noexcept { return _balances; }
	};

	template<>
	paper_trading_config from_json<paper_trading_config>(const json_document& json);

	template<>
	void to_json<paper_trading_config>(const paper_trading_config& config, json_writer& writer);
}