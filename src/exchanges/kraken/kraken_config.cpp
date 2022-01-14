#include "kraken_config.h"

namespace cb
{
	kraken_config::kraken_config(std::string publicKey, std::string privateKey)
		: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
	{}

	kraken_config kraken_config::deserialize(json_wrapper& json)
	{
		std::string publicKey = json.document()["publicKey"].GetString();
		std::string privateKey = json.document()["privateKey"].GetString();

		return kraken_config{ std::move(publicKey), std::move(privateKey) };
	}

	std::string kraken_config::serialize() const
	{
		return "";
	}
}