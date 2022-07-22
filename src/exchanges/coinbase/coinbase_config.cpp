#include "coinbase_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view API_KEY = "apiKey";
		static constexpr std::string_view API_SECRET = "apiSecret";
		static constexpr std::string_view API_PASSPHRASE = "apiPassphrase";
	}
}

namespace mb
{
	coinbase_config::coinbase_config()
		: _apiKey{}, _apiSecret{}, _apiPassphrase{}
	{};

	coinbase_config::coinbase_config(std::string apiKey, std::string apiSecret, std::string apiPassphrase)
		: 
		_apiKey{ std::move(apiKey) }, 
		_apiSecret{ std::move(apiSecret) }, 
		_apiPassphrase{std::move(apiPassphrase)}
	{}

	template<>
	coinbase_config from_json<coinbase_config>(const json_document& json)
	{
		std::string apiKey{ json.get<std::string>(json_property_names::API_KEY) };
		std::string apiSecret{ json.get<std::string>(json_property_names::API_SECRET) };
		std::string apiPassphrase{ json.get<std::string>(json_property_names::API_PASSPHRASE) };

		return coinbase_config{ std::move(apiKey), std::move(apiSecret), std::move(apiPassphrase) };
	}

	template<>
	void to_json<coinbase_config>(const coinbase_config& config, json_writer& writer)
	{
		writer.add(json_property_names::API_KEY, config.api_key());
		writer.add(json_property_names::API_SECRET, config.api_secret());
		writer.add(json_property_names::API_PASSPHRASE, config.api_passphrase());
	}
}