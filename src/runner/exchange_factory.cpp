#include "exchange_factory.h"
#include "config_file_reader.h"
#include "networking/http/http_service.h"
#include "logging/logger.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/coinbase/coinbase.h"
#include "exchanges/digifinex/digifinex.h"
#include "exchanges/dextrade/dextrade.h"

namespace
{
	using namespace mb;

	template<typename Config, typename MakeApi>
	std::unique_ptr<exchange> create_api(std::shared_ptr<websocket_client> websocketClient, MakeApi makeApi)
	{
		Config config = load_or_create_config<Config>();
		return makeApi(std::move(config), websocketClient);
	}

	std::unique_ptr<exchange> create_api_from_id(
		std::string_view identifier,
		std::shared_ptr<websocket_client> websocketClient)
	{
		if (identifier == exchange_ids::KRAKEN)
		{
			return create_api<kraken_config>(websocketClient, make_kraken);
		}
		if (identifier == exchange_ids::COINBASE)
		{
			return create_api<coinbase_config>(
				websocketClient, 
				[](coinbase_config config, std::shared_ptr<websocket_client> client) { return make_coinbase(std::move(config), client); });
		}
		if (identifier == exchange_ids::DIGIFINEX)
		{
			return create_api<digifinex_config>(websocketClient, make_digifinex);
		}
		if (identifier == exchange_ids::DEXTRADE)
		{
			return create_api<dextrade_config>(websocketClient, make_dextrade);
		}

		return nullptr;
	}

	template<typename ExchangeIds>
	std::vector<std::shared_ptr<exchange>> create_exchanges(
		const ExchangeIds& exchangeIds,
		std::shared_ptr<websocket_client> websocketClient)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		exchanges.reserve(exchangeIds.size());

		logger& log{ logger::instance() };

		for (auto& exchangeId : exchangeIds)
		{
			log.info("Creating exchange API: {}", exchangeId);

			std::unique_ptr<exchange> api = create_api_from_id(exchangeId, websocketClient);

			if (!api)
			{
				logger::instance().warning("Exchange '{}' is not supported", exchangeId);
				continue;
			}

			log.info("{} API created successfully. Testing connection...", exchangeId);

			exchange_status status = api->get_status();
			std::string status_string = to_string(status);

			if (status == exchange_status::ONLINE)
			{
				log.info("Exchange status: {}", status_string);
			}
			else
			{
				log.warning("Exchange status: {}", status_string);
			}

			exchanges.emplace_back(std::move(api));
		}

		return exchanges;
	}
}

namespace mb::internal
{
	std::vector<std::shared_ptr<exchange>> create_exchange_apis(const runner_config& runnerConfig)
	{
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>(runnerConfig.websocket_timeout());
		http_service::set_timeout(runnerConfig.http_timeout());

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			return ::create_exchanges(exchange_ids::all(), websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), websocketClient);
		}
	}
}