#pragma once

#include "common/json/json.h"

namespace mb
{
	class coinbase_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;
		std::string _apiPassphrase;

	public:
		coinbase_config();
		coinbase_config(std::string apiKey, std::string apiSecret, std::string apiPassphrase);

		static std::string name() noexcept { return "coinbase"; }

		const std::string& api_key() const noexcept { return _apiKey; }
		const std::string& api_secret() const noexcept { return _apiSecret; }
		const std::string& api_passphrase() const noexcept { return _apiPassphrase; }
	};

	template<>
	coinbase_config from_json<coinbase_config>(const json_document& json);

	template<>
	void to_json<coinbase_config>(const coinbase_config& config, json_writer& writer);
}