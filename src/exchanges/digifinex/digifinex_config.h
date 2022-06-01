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
		constexpr digifinex_config()
			: _apiKey{}, _apiSecret{}
		{};

		constexpr digifinex_config(std::string apiKey, std::string apiSecret)
			: _apiKey{ std::move(apiKey) }, _apiSecret{ std::move(apiSecret) }
		{}

		static constexpr std::string name() noexcept { return "digifinex"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& api_secret() const noexcept { return _apiSecret; }
	};

	template<>
	digifinex_config from_json<digifinex_config>(const json_document& json);

	template<>
	void to_json<digifinex_config>(const digifinex_config& config, json_writer& writer);
}