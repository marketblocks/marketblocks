#pragma once

#include "common/json/json.h"

namespace mb
{
	class binance_config
	{
	private:
		std::string _apiKey;
		std::string _secretKey;

	public:
		binance_config();
		binance_config(std::string publicKey, std::string privateKey);

		static std::string name() noexcept { return "binance"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& secret_key() const noexcept { return _secretKey; }
	};

	template<>
	binance_config from_json<binance_config>(const json_document& json);

	template<>
	void to_json<binance_config>(const binance_config& config, json_writer& writer);
}