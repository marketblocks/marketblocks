#include "kraken_config.h"

namespace cb
{
	kraken_config::kraken_config(std::string publicKey, std::string privateKey)
		: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
	{}

	kraken_config kraken_config::create_default()
	{
		return kraken_config{ "", "" };
	}

	kraken_config kraken_config::deserialize(json_document& json)
	{
		std::string publicKey = json.get<std::string>("publicKey");
		std::string privateKey = json.get<std::string>("privateKey");

		return kraken_config{ std::move(publicKey), std::move(privateKey) };
	}

	std::string kraken_config::serialize() const
	{
		return "";
	}
}