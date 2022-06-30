#include "runner_config.h"
#include "logging/logger.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	static constexpr int DEFAULT_WEBSOCKET_TIMEOUT = 5000;
	static constexpr int DEFAULT_HTTP_TIMEOUT = 5000;

	namespace json_property_names
	{
		static constexpr std::string_view EXCHANGE_IDS = "exchangeIds";
		static constexpr std::string_view RUN_MODE = "runMode";
		static constexpr std::string_view WEBSOCKET_TIMEOUT = "websocketTimeout";
		static constexpr std::string_view HTTP_TIMEOUT = "httpTimeout";
		static constexpr std::string_view RUN_INTERVAL = "runInterval";
		static constexpr std::string_view SYNC_TIME = "syncTime";
	}

	namespace run_mode_strings
	{
		static constexpr std::string_view LIVE = "live";
		static constexpr std::string_view LIVE_TEST = "live_test";
		static constexpr std::string_view BACK_TEST = "back_test";
		static constexpr std::string_view UNKNOWN = "unknown";
	}
}

namespace mb
{
	std::string_view to_string(run_mode runMode)
	{
		switch (runMode)
		{
		case run_mode::LIVE:
			return run_mode_strings::LIVE;
		case run_mode::LIVETEST:
			return run_mode_strings::LIVE_TEST;
		case run_mode::BACKTEST:
			return run_mode_strings::BACK_TEST;
		default:
			return run_mode_strings::UNKNOWN;
		}
	}

	run_mode run_mode_from_string(std::string_view runMode)
	{
		if (runMode == run_mode_strings::LIVE)
		{
			return run_mode::LIVE;
		}
		else if (runMode == run_mode_strings::LIVE_TEST)
		{
			return run_mode::LIVETEST;
		}
		else if (runMode == run_mode_strings::BACK_TEST)
		{
			return run_mode::BACKTEST;
		}

		return run_mode::UNKNOWN;
	}

	runner_config::runner_config()
		: runner_config{ {}, run_mode::LIVETEST, DEFAULT_WEBSOCKET_TIMEOUT, DEFAULT_HTTP_TIMEOUT, 0, false }
	{}

	runner_config::runner_config(
		std::vector<std::string> exchangeIds,
		run_mode runMode,
		int websocketTimeout,
		int httpTimeout,
		int runInterval,
		bool syncTime)
		:
		_exchangeIds{ std::move(exchangeIds) },
		_runMode{ runMode },
		_websocketTimeout{ websocketTimeout },
		_httpTimeout{ httpTimeout },
		_runInterval{ runInterval },
		_syncTime{ syncTime }
	{
		validate();
	}

	void runner_config::validate()
	{
		logger& log{ logger::instance() };

		if (_exchangeIds.empty())
		{
			log.warning("Exchange list is empty, all supported exchanges will be used");
		}

		if (_runMode == run_mode::UNKNOWN)
		{
			throw mb_exception{ std::format("Run mode not recognized. Options are: {0}, {1}, {2}", run_mode_strings::LIVE, run_mode_strings::LIVE_TEST, run_mode_strings::BACK_TEST) };
		}

		if (_runInterval < 0)
		{
			_runInterval = 0;
			log.warning("Run interval cannot be less than zero");
		}
	}

	template<>
	runner_config from_json(const json_document& json)
	{
		return runner_config
		{
			json.get<std::vector<std::string>>(json_property_names::EXCHANGE_IDS),
			run_mode_from_string(json.get<std::string>(json_property_names::RUN_MODE)),
			json.get<int>(json_property_names::WEBSOCKET_TIMEOUT),
			json.get<int>(json_property_names::HTTP_TIMEOUT),
			json.get<int>(json_property_names::RUN_INTERVAL),
			json.get<bool>(json_property_names::SYNC_TIME)
		};
	}

	template<>
	void to_json(const runner_config& config, json_writer& writer)
	{
		writer.add(json_property_names::EXCHANGE_IDS, config.exchange_ids());
		writer.add(json_property_names::RUN_MODE, to_string(config.runmode()));
		writer.add(json_property_names::WEBSOCKET_TIMEOUT, config.websocket_timeout());
		writer.add(json_property_names::HTTP_TIMEOUT, config.http_timeout());
		writer.add(json_property_names::RUN_INTERVAL, config.run_interval());
		writer.add(json_property_names::SYNC_TIME, config.sync_time());
	}
}