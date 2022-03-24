#include <stdexcept>
#include <functional>

#include "runner_initialise.h"
#include "initialisation_error.h"
#include "exchanges/exchange_ids.h"
#include "exchanges/kraken/kraken.h"
#include "exchanges/coinbase/coinbase.h"
#include "exchanges/exchange_assemblers.h"
#include "exchanges/exchange_status.h"
#include "common/file/file.h"
#include "common/exceptions/cb_exception.h"
#include "logging/logger.h"
#include "networking/websocket/websocket_client.h"

namespace
{
	using namespace cb;

	template<typename Config>
	Config get_config()
	{
		logger& log{ logger::instance() };
		std::string configName{ Config::name() };

		log.info("Reading config file: {}", configName);

		try
		{
			Config config = load_or_create_config<Config>();

			log.info("{} created successfully", configName);
			return config;
		}
		catch (const std::exception& e)
		{
			log.error("Error occurred reading {0}: {1}, using default values", configName, e.what());
			return Config{};
		}
	}

	typedef std::shared_ptr<exchange>(*exchange_assembler)(std::unique_ptr<exchange> api);
	exchange_assembler select_assembler(run_mode runMode)
	{
		switch (runMode)
		{
			case run_mode::LIVE:
				return assemble_live;
			case run_mode::LIVETEST:
				return [](std::unique_ptr<exchange> api)
				{
					paper_trading_config config{ get_config<paper_trading_config>() };
					return assemble_live_test(std::move(api), std::move(config));
				};
			default:
				throw initialisation_error{ "Run mode not supported" };
		}
	}

	std::unique_ptr<exchange> create_api_from_id(
		std::string_view identifier,
		std::shared_ptr<websocket_client> websocketClient)
	{
		if (identifier == exchange_ids::KRAKEN)
		{
			kraken_config config = get_config<kraken_config>();
			return make_kraken(std::move(config), websocketClient);
		}
		else if (identifier == exchange_ids::COINBASE)
		{
			coinbase_config config = get_config<coinbase_config>();
			return make_coinbase(std::move(config), websocketClient);
		}
		else
		{
			return nullptr;
		}
	}

	template<typename ExchangeIds>
	void log_exchange_list(const ExchangeIds& exchangeIds, logger& log)
	{
		std::string exchangeList;

		for (auto& id : exchangeIds)
		{
			exchangeList.append("\n");
			exchangeList.append(id);
		}

		log.info("Found {0} exchange(s):{1}", exchangeIds.size(), exchangeList);
	}

	template<typename ExchangeIds>
	std::vector<std::shared_ptr<cb::exchange>> create_exchanges(
		const ExchangeIds& exchangeIds,
		const exchange_assembler& assembler,
		std::shared_ptr<cb::websocket_client> websocketClient)
	{
		std::vector<std::shared_ptr<cb::exchange>> exchanges;
		exchanges.reserve(exchangeIds.size());

		logger& log{ logger::instance() };
		log_exchange_list(exchangeIds, log);

		for (auto& exchangeId : exchangeIds)
		{
			log.info("Creating exchange API: {}", exchangeId);

			std::unique_ptr<cb::exchange> api = create_api_from_id(exchangeId, websocketClient);

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

			exchanges.emplace_back(assembler(std::move(api)));
		}

		return exchanges;
	}
}

namespace cb::internal
{
	runner_config get_runner_config()
	{
		return get_config<runner_config>();
	}

	std::vector<std::shared_ptr<exchange>> create_exchanges(const runner_config& runnerConfig)
	{
		std::vector<std::shared_ptr<exchange>> exchanges;
		std::shared_ptr<websocket_client> websocketClient = std::make_shared<websocket_client>(runnerConfig.websocket_timeout());
		exchange_assembler assembler = select_assembler(runnerConfig.runmode());

		http_service::set_timeout(runnerConfig.http_timeout());

		logger::instance().info("Creating exchange APIs...");

		if (runnerConfig.exchange_ids().empty())
		{
			return ::create_exchanges(exchange_ids::all(), assembler, websocketClient);
		}
		else
		{
			return ::create_exchanges(runnerConfig.exchange_ids(), assembler, websocketClient);
		}
	}
}