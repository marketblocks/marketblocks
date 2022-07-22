#include "exchange_factory.h"
#include "networking/http/http_service.h"
#include "logging/logger.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/coinbase/coinbase.h"
#include "exchanges/bybit/bybit.h"
#include "exchanges/digifinex/digifinex.h"
#include "exchanges/dextrade/dextrade.h"
#include "exchanges/binance/binance.h"

namespace
{
	using namespace mb;

	std::unique_ptr<exchange> create_api_from_id(std::string_view identifier)
	{
		if (identifier == exchange_ids::KRAKEN)
		{
			return create_exchange_api<kraken_api>();
		}
		if (identifier == exchange_ids::COINBASE)
		{
			return create_exchange_api<coinbase_api>();
		}
		if (identifier == exchange_ids::BYBIT)
		{
			return create_exchange_api<bybit_api>();
		}
		if (identifier == exchange_ids::DIGIFINEX)
		{
			return create_exchange_api<digifinex_api>();
		}
		if (identifier == exchange_ids::DEXTRADE)
		{
			return create_exchange_api<dextrade_api>();
		}
		if (identifier == exchange_ids::BINANCE)
		{
			return create_exchange_api<binance_api>();
		}

		return nullptr;
	}

	template<typename ExchangeIds>
	std::vector<std::shared_ptr<exchange>> create_exchanges(const ExchangeIds& exchangeIds)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		exchanges.reserve(exchangeIds.size());

		logger& log{ logger::instance() };

		for (auto& exchangeId : exchangeIds)
		{
			log.info("Creating exchange API: {}", exchangeId);

			std::unique_ptr<exchange> api = create_api_from_id(exchangeId);

			if (!api)
			{
				logger::instance().warning("Exchange '{}' is not supported", exchangeId);
				continue;
			}

			log.info("{} API created successfully. Testing connection...", exchangeId);

			exchange_status status = api->get_status();
			std::string status_string{ to_string(status) };

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
		http_service::set_timeout(runnerConfig.http_timeout());
		websocket_client::instance().set_open_handshake_timeout(runnerConfig.websocket_timeout());

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			return ::create_exchanges(exchange_ids::all());
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids());
		}
	}
}