#pragma once

#include "common/json/json.h"

namespace mb
{
	class digifinex_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;

	public:
		digifinex_config();
		digifinex_config(std::string apiKey, std::string apiSecret);

		static std::string name() noexcept { return "digifinex"; }

		const std::string& api_key() const noexcept { return _apiKey; }
		const std::string& api_secret() const noexcept { return _apiSecret; }
	};

	template<>
	digifinex_config from_json<digifinex_config>(const json_document& json);

	template<>
	void to_json<digifinex_config>(const digifinex_config& config, json_writer& writer);
}