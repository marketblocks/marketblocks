#include "dextrade.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/exceptions/not_implemented_exception.h"
#include "common/file/config_file_reader.h"

namespace
{
	using namespace mb;

	int get_trade_type(order_type orderType)
	{
		switch (orderType)
		{
		case order_type::LIMIT:
			return 0;
		case order_type::MARKET:
			return 1;
		}
	}

	int get_action_type(trade_action tradeAction)
	{
		return tradeAction == trade_action::BUY
			? 0
			: 1;
	}
}

namespace mb
{
	dextrade_api::dextrade_api(
		dextrade_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream)
		: 
		exchange{ exchange_ids::DEXTRADE, websocketStream },
		_publicKey{ config.public_key() },
		_privateKey{ config.private_key() },
		_httpService{ std::move(httpService) }
	{}

	std::string dextrade_api::compute_api_sign(const json_document& jsonQuery) const
	{
		std::string queryValues;

		for (auto it = jsonQuery.begin(); it != jsonQuery.end(); ++it)
		{
			json_element item{ it.value() };

			if (item.type() == json_value_type::INT)
			{
				queryValues.append(std::to_string(item.get<int>()));
			}
			else
			{
				queryValues.append(item.get<std::string>());
			}
		}

		queryValues.append(_privateKey);
		return hex_encode(sha256(queryValues));
	}

	exchange_status dextrade_api::get_status() const
	{
		std::vector<tradable_pair> pairs{ get_tradable_pairs() };

		if (pairs.empty())
		{
			return exchange_status::OFFLINE;
		}

		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> dextrade_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("symbols", dextrade::read_tradable_pairs);
	}

	std::vector<ohlcv_data> dextrade_api::get_ohlcv(const tradable_pair& tradablePair, ohlcv_interval interval, int count) const
	{
		throw not_implemented_exception{ "dextrade::get_ohlcv" };
	}

	double dextrade_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_public_request<double>("ticker", dextrade::read_price, query);
	}

	order_book_state dextrade_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string query = url_query_builder{}
			.add_parameter("pair", tradablePair.to_string())
			.to_string();

		return send_public_request<order_book_state>("book", dextrade::read_order_book, query);
	}

	double dextrade_api::get_fee(const tradable_pair& tradablePair) const
	{
		return 0.2;
	}

	unordered_string_map<double> dextrade_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>("balances", dextrade::read_balances);
	}

	std::vector<order_description> dextrade_api::get_open_orders() const
	{
		throw not_implemented_exception{ "dextrade::get_open_orders" };
	}

	std::vector<order_description> dextrade_api::get_closed_orders() const
	{
		throw not_implemented_exception{ "dextrade::get_closed_orders" };
	}

	std::string dextrade_api::add_order(const order_request& description)
	{
		json_writer jsonWriter;
		jsonWriter.add("type_trade", get_trade_type(description.order_type()));
		jsonWriter.add("type", get_action_type(description.action()));
		jsonWriter.add("rate", std::to_string(description.asset_price()));
		jsonWriter.add("volume", std::to_string(description.volume()));
		jsonWriter.add("pair", description.pair().to_string());

		return send_private_request<std::string>("create-order", dextrade::read_add_order, jsonWriter);
	}

	void dextrade_api::cancel_order(std::string_view orderId)
	{
		throw not_implemented_exception{ "dextrade::cancel_order" };
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<dextrade_api>(bool testing)
	{
		return std::make_unique<dextrade_api>(
			internal::load_or_create_config<dextrade_config>(),
			std::make_unique<http_service>(),
			nullptr);
	}
}