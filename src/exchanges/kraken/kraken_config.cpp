#include "kraken_config.h"
#include "logging/logger.h"
#include "common/exceptions/not_implemented_exception.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view PUBLIC_KEY = "publicKey";
		static constexpr std::string_view PRIVATE_KEY = "privateKey";
	}
}

namespace cb
{
	kraken_config::kraken_config(std::string publicKey, std::string privateKey)
		: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }, _httpRetries{ DEFAULT_RETRIES }
	{
		validate();
	}

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
		std::string publicKey{ json.get<std::string>(json_property_names::PUBLIC_KEY) };
		std::string privateKey{ json.get<std::string>(json_property_names::PRIVATE_KEY) };

		return kraken_config{ std::move(publicKey), std::move(privateKey) };
	}

	template<>
	void to_json(const kraken_config& config, json_writer& writer)
	{
		writer.add(json_property_names::PUBLIC_KEY, config.public_key());
		writer.add(json_property_names::PRIVATE_KEY, config.private_key());
	}
}