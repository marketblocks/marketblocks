#pragma once

#include "common/json/json.h"

namespace mb
{
	class bybit_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;
		double _fee;

	public:
		bybit_config();
		bybit_config(std::string apiKey, std::string apiSecret, double fee);

		static std::string name() noexcept { return "bybit"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& api_secret() const noexcept { return _apiSecret; }
		constexpr double fee() const noexcept { return _fee; }
	};

	template<>
	bybit_config from_json<bybit_config>(const json_document& json);

	template<>
	void to_json<bybit_config>(const bybit_config& config, json_writer& writer);
}