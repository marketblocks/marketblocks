#include "websocket_stream_constants.h"
#include "common/types/unordered_string_map.h"
#include "common/utils/containerutils.h"

namespace mb
{
	ohlcv_interval parse_ohlcv_interval(std::string_view string)
	{
		static unordered_string_map<ohlcv_interval> ohlcvIntervalLookup
		{
			{ "1m", ohlcv_interval::M1 },
			{ "5m", ohlcv_interval::M5 },
			{ "15m", ohlcv_interval::M15 },
			{ "1h", ohlcv_interval::H1 },
			{ "1d", ohlcv_interval::D1 },
			{ "1w", ohlcv_interval::W1 }
		};

		return find_or_default(ohlcvIntervalLookup, string, ohlcv_interval::UNKNOWN);
	}

	std::string to_string(ohlcv_interval interval)
	{
		static std::unordered_map<ohlcv_interval, std::string> ohlcvIntervalLookup
		{
			{ ohlcv_interval::M1, "1m" },
			{ ohlcv_interval::M5, "5m" },
			{ ohlcv_interval::M15, "15m" },
			{ ohlcv_interval::H1, "1h" },
			{ ohlcv_interval::D1, "1d" },
			{ ohlcv_interval::W1, "1w" }
		};

		return find_or_default(ohlcvIntervalLookup, interval, std::string{});
	}
}