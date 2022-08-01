#include "websocket_stream_constants.h"
#include "common/utils/containerutils.h"

namespace mb
{
	ohlcv_interval parse_ohlcv_interval(std::string_view string)
	{
		static std::unordered_map<std::string,ohlcv_interval> ohlcvIntervalLookup
		{
			{ "1m", ohlcv_interval::M1 },
			{ "5m", ohlcv_interval::M5 },
			{ "15m", ohlcv_interval::M15 },
			{ "1h", ohlcv_interval::H1 },
			{ "1d", ohlcv_interval::D1 },
			{ "1w", ohlcv_interval::W1 }
		};

		auto it = ohlcvIntervalLookup.find(string.data());
		if (it == ohlcvIntervalLookup.end())
		{
			return ohlcv_interval::UNKNOWN;
		}

		return it->second;
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

		return find_or_default<std::string>(ohlcvIntervalLookup, interval);
	}

	int to_seconds(ohlcv_interval interval)
	{
		switch (interval)
		{
		case ohlcv_interval::M1:
			return 60;
		case ohlcv_interval::M5:
			return 300;
		case ohlcv_interval::M15:
			return 900;
		case ohlcv_interval::H1:
			return 3600;
		case ohlcv_interval::D1:
			return 86400;
		case ohlcv_interval::W1:
			return 604800;
		}
	}

	int to_minutes(ohlcv_interval interval)
	{
		return to_seconds(interval) / 60;
	}

	ohlcv_interval from_seconds(int seconds)
	{
		switch (seconds)
		{
		case 60:
			return ohlcv_interval::M1;
		case 300:
			return ohlcv_interval::M5;
		case 900:
			return ohlcv_interval::M15;
		case 3600:
			return ohlcv_interval::H1;
		case 86400:
			return ohlcv_interval::D1;
		case 604800:
			return ohlcv_interval::W1;
		default:
			return ohlcv_interval::UNKNOWN;
		}

	}

	ohlcv_interval from_minutes(int minutes)
	{
		return from_seconds(minutes * 60);
	}
}