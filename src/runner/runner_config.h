#pragma once

#include <vector>

#include "common/json/json.h"

namespace mb
{
	enum class run_mode
	{
		LIVE, LIVETEST, BACKTEST
	};

	class runner_config
	{
	private:
		std::vector<std::string> _exchangeIds;
		run_mode _runMode;
		int _websocketTimeout;
		int _httpTimeout;

		void validate();

	public:
		runner_config();

		runner_config(
			std::vector<std::string> exchangeIds,
			run_mode runMode,
			int websocketTimeout,
			int httpTimeout);
			
		static constexpr std::string name() noexcept { return "runner"; }
		
		constexpr const std::vector<std::string>& exchange_ids() const noexcept { return _exchangeIds; }
		constexpr run_mode runmode() const noexcept { return _runMode; }
		constexpr int websocket_timeout() const noexcept { return _websocketTimeout; }
		constexpr int http_timeout() const noexcept { return _httpTimeout; }
	};

	template<>
	runner_config from_json(const json_document& json);

	template<>
	void to_json(const runner_config& config, json_writer& writer);
}