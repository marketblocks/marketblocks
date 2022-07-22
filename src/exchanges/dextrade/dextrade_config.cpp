#include "dextrade_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view PUBLIC_KEY = "publicKey";
		static constexpr std::string_view PRIVATE_KEY = "privateKey";
	}
}

namespace mb
{
	dextrade_config::dextrade_config()
		: _publicKey{}, _privateKey{}
	{};

	dextrade_config::dextrade_config(std::string publicKey, std::string privateKey)
		: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
	{}

	template<>
	dextrade_config from_json<dextrade_config>(const json_document& json)
	{
		return dextrade_config
		{
			json.get<std::string>(json_property_names::PUBLIC_KEY),
			json.get<std::string>(json_property_names::PRIVATE_KEY)
		};
	}

	template<>
	void to_json<dextrade_config>(const dextrade_config& config, json_writer& writer)
	{
		writer.add(json_property_names::PUBLIC_KEY, config.public_key());
		writer.add(json_property_names::PRIVATE_KEY, config.private_key());
	}
}