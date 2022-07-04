#include "dextrade_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view API_KEY = "apiKey";
		static constexpr std::string_view API_SECRET = "apiSecret";
	}
}

namespace mb
{
	template<>
	dextrade_config from_json<dextrade_config>(const json_document& json)
	{
		return dextrade_config
		{
			json.get<std::string>(json_property_names::API_KEY),
			json.get<std::string>(json_property_names::API_SECRET)
		};
	}

	template<>
	void to_json<dextrade_config>(const dextrade_config& config, json_writer& writer)
	{
		writer.add(json_property_names::API_KEY, config.api_key());
		writer.add(json_property_names::API_SECRET, config.api_secret());
	}
}