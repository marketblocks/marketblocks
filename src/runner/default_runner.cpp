#include "default_runner.h"
#include "exchange_assemblers.h"
#include "config_file_reader.h"
#include "networking/http/http_service.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/coinbase/coinbase.h"

namespace
{
	using namespace mb;

	std::unique_ptr<exchange_assembler> select_assembler(run_mode runMode)
	{
		switch (runMode)
		{
		case run_mode::LIVE:
			return std::make_unique<assemble_live>();
		case run_mode::LIVETEST:
			return std::make_unique<assemble_live_test>(
				load_or_create_config<paper_trading_config>());
		default:
			throw mb_exception{ "Run mode not supported" };
		}
	}

	std::unique_ptr<exchange> create_api_from_id(
		std::string_view identifier,
		std::shared_ptr<websocket_client> websocketClient)
	{
		if (identifier == exchange_ids::KRAKEN)
		{
			kraken_config config = load_or_create_config<kraken_config>();
			return make_kraken(std::move(config), websocketClient);
		}
		else if (identifier == exchange_ids::COINBASE)
		{
			coinbase_config config = load_or_create_config<coinbase_config>();
			return make_coinbase(std::move(config), websocketClient);
		}
		else
		{
			return nullptr;
		}
	}

	template<typename ExchangeIds>
	std::vector<std::shared_ptr<exchange>> create_exchanges(
		const ExchangeIds& exchangeIds,
		std::unique_ptr<exchange_assembler> assembler,
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

			exchanges.emplace_back(assembler->assemble(std::move(api)));
		}

		return exchanges;
	}
}

namespace mb::internal
{
	std::vector<std::shared_ptr<exchange>> do_create_exchanges(const runner_config& runnerConfig)
	{
		std::unique_ptr<exchange_assembler> assembler = select_assembler(runnerConfig.runmode());
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>(runnerConfig.websocket_timeout());

		http_service::set_timeout(runnerConfig.http_timeout());

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			return ::create_exchanges(exchange_ids::all(), std::move(assembler), websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), std::move(assembler), websocketClient);
		}
	}
}