#include "bybit_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view API_KEY = "apiKey";
		static constexpr std::string_view API_SECRET = "apiSecret";
		static constexpr std::string_view FEE = "fee";
	}
}

namespace mb
{
	template<>
	bybit_config from_json<bybit_config>(const json_document& json)
	{
		return bybit_config
		{
			json.get<std::string>(json_property_names::API_KEY),
			json.get<std::string>(json_property_names::API_SECRET),
			json.get<double>(json_property_names::FEE)
		};
	}

	template<>
	void to_json<bybit_config>(const bybit_config& config, json_writer& writer)
	{
		writer.add(json_property_names::API_KEY, config.api_key());
		writer.add(json_property_names::API_SECRET, config.api_secret());
		writer.add(json_property_names::FEE, config.fee());
	}
}