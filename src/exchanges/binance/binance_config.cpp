#include "binance_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view API_KEY = "apiKey";
		static constexpr std::string_view SECRET_KEY = "secretKey";
	}
}

namespace mb
{
	template<>
	binance_config from_json<binance_config>(const json_document& json)
	{
		return binance_config
		{
			json.get<std::string>(json_property_names::API_KEY),
			json.get<std::string>(json_property_names::SECRET_KEY)
		};
	}

	template<>
	void to_json<binance_config>(const binance_config& config, json_writer& writer)
	{
		writer.add(json_property_names::API_KEY, config.api_key());
		writer.add(json_property_names::SECRET_KEY, config.secret_key());
	}
}