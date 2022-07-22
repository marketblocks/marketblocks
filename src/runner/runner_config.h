#pragma once

#include <vector>

#include "common/json/json.h"

namespace mb
{
	enum class run_mode
	{
		LIVE, LIVETEST, BACKTEST, UNKNOWN
	};

	std::string_view to_string(run_mode runMode);
	run_mode run_mode_from_string(std::string_view runMode);

	class runner_config
	{
	private:
		std::vector<std::string> _exchangeIds;
		run_mode _runMode;
		int _websocketTimeout;
		int _httpTimeout;
		int _runInterval;
		bool _syncTime;

		void validate();

	public:
		runner_config();

		runner_config(
			std::vector<std::string> exchangeIds,
			run_mode runMode,
			int websocketTimeout,
			int httpTimeout,
			int runInterval,
			bool syncTime);
			
		static std::string name() noexcept { return "runner"; }
		
		constexpr const std::vector<std::string>& exchange_ids() const noexcept { return _exchangeIds; }
		constexpr run_mode runmode() const noexcept { return _runMode; }
		constexpr int websocket_timeout() const noexcept { return _websocketTimeout; }
		constexpr int http_timeout() const noexcept { return _httpTimeout; }
		constexpr int run_interval() const noexcept { return _runInterval; }
		constexpr bool sync_time() const noexcept { return _syncTime; }
	};

	template<>
	runner_config from_json(const json_document& json);

	template<>
	void to_json(const runner_config& config, json_writer& writer);
}