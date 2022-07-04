#pragma once

#include "common/json/json.h"

namespace mb
{
	class dextrade_config
	{
	private:
		std::string _apiKey;
		std::string _apiSecret;

	public:
		constexpr dextrade_config()
			: _apiKey{}, _apiSecret{}
		{};

		constexpr dextrade_config(std::string apiKey, std::string apiSecret)
			: _apiKey{ std::move(apiKey) }, _apiSecret{ std::move(apiSecret) }
		{}

		static constexpr std::string name() noexcept { return "dextrade"; }

		constexpr const std::string& api_key() const noexcept { return _apiKey; }
		constexpr const std::string& api_secret() const noexcept { return _apiSecret; }
	};

	template<>
	dextrade_config from_json<dextrade_config>(const json_document& json);

	template<>
	void to_json<dextrade_config>(const dextrade_config& config, json_writer& writer);
}