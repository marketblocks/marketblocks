#include "kraken_config.h"
#include "logging/logger.h"
#include "common/exceptions/not_implemented_exception.h"

namespace cb
{
	kraken_config::kraken_config(std::string publicKey, std::string privateKey)
		: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
	{
		validate();
	}

	kraken_config::kraken_config()
		: kraken_config{ "", "" }
	{}

	void kraken_config::validate()
	{
		logger& log{ logger::instance() };

		if (_publicKey.empty())
		{
			log.warning("Authentication public key is empty, authenticated endpoints will be unavailable");
		}

		if (_privateKey.empty())
		{
			log.warning("Authentication private key is empty, authenticated endpoints will be unavailable");
		}
	}

	template<>
	kraken_config from_json(const json_document& json)
	{
		std::string publicKey = json.get<std::string>("publicKey");
		std::string privateKey = json.get<std::string>("privateKey");

		return kraken_config{ std::move(publicKey), std::move(privateKey) };
	}

	template<>
	json_document to_json(const kraken_config& config)
	{
		throw not_implemented_exception{ "kraken_config::to_json" };
	}
}