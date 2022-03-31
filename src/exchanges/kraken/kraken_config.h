#pragma once

#include <string>

#include "common/json/json.h"

namespace mb
{
	class kraken_config
	{
	private:
		static constexpr int DEFAULT_RETRIES = 3;

		std::string _publicKey;
		std::string _privateKey;
		int _httpRetries;

		void validate();

	public:
		constexpr kraken_config()
			: _publicKey{}, _privateKey{}, _httpRetries{ DEFAULT_RETRIES }
		{}

		kraken_config(std::string publicKey, std::string privateKey);

		static constexpr std::string name() noexcept { return "kraken"; }

		constexpr const std::string& public_key() const noexcept { return _publicKey; }
		constexpr const std::string& private_key() const noexcept { return _privateKey; }
		constexpr int http_retries() const noexcept { return _httpRetries; }
	};

	template<>
	kraken_config from_json<kraken_config>(const json_document& json);

	template<>
	void to_json<kraken_config>(const kraken_config& config, json_writer& writer);
}