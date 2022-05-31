#pragma once

#include "common/json/json.h"

namespace mb
{
	class bybit_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;

	public:
		constexpr bybit_config()
			: _apiKey{}, _apiSecret{}
		{};

		constexpr bybit_config(std::string apiKey, std::string apiSecret)
			: _apiKey{ std::move(apiKey) }, _apiSecret{ std::move(apiSecret) }
		{}

		static constexpr std::string name() noexcept { return "bybit"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& api_secret() const noexcept { return _apiSecret; }
	};

	template<>
	bybit_config from_json<bybit_config>(const json_document& json);

	template<>
	void to_json<bybit_config>(const bybit_config& config, json_writer& writer);
}