#pragma once

#include "common/file/json.h"

namespace cb
{
	class coinbase_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;
		std::string _apiPassphrase;

	public:
		constexpr coinbase_config()
			: _apiKey{}, _apiSecret{}, _apiPassphrase{}
		{};

		coinbase_config(std::string apiKey, std::string apiSecret, std::string apiPassphrase);

		static constexpr std::string name() noexcept { return "coinbaseConfig"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& api_secret() const noexcept { return _apiSecret; }
		constexpr const std::string& api_passphrase() const noexcept { return _apiPassphrase; }
	};

	template<>
	coinbase_config from_json<coinbase_config>(const json_document& json);

	template<>
	void to_json<coinbase_config>(const coinbase_config& config, json_writer& writer);
}