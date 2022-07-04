#include "coinbase.h"
#include "coinbase_results.h"
#include "common/utils/timeutils.h"
#include "common/security/hash.h"
#include "common/security/encoding.h"
#include "common/file/config_file_reader.h"

namespace
{
	using namespace mb;

	constexpr std::string_view select_base_url(bool enableTesting)
	{
		constexpr std::string_view LIVE_BASE_URL = "https://api.exchange.coinbase.com";
		constexpr std::string_view SANDBOX_BASE_URL = "https://api-public.sandbox.exchange.coinbase.com";

		if (enableTesting)
		{
			return SANDBOX_BASE_URL;
		}

		return LIVE_BASE_URL;
	}

	constexpr std::string to_string(order_type orderType)
	{
		switch (orderType)
		{
		case order_type::LIMIT:
			return "limit";
		case order_type::MARKET:
			return "market";
		case order_type::STOP_LOSS:
			return "loss";
		case order_type::TAKE_PROFIT:
			return "entry";
		default:
			throw mb_exception{ "Unknown order type" };
		}
	}

	constexpr bool is_stop_order(order_type orderType)
	{
		return orderType == order_type::STOP_LOSS || orderType == order_type::TAKE_PROFIT;
	}
}

namespace mb
{
	coinbase_api::coinbase_api(
		coinbase_config config,
		std::unique_ptr<http_service> httpService,
		std::shared_ptr<websocket_stream> websocketStream,
		bool enableTesting)
		:
		exchange{ exchange_ids::COINBASE, std::move(websocketStream) },
		_baseUrl{ select_base_url(enableTesting) },
		_userAgentId{ get_timestamp() },
		_apiKey{ config.api_key() },
		_decodedApiSecret{ b64_decode(config.api_secret()) },
		_apiPassphrase{ config.api_passphrase() },
		_httpService{ std::move(httpService) }
	{}

	std::string coinbase_api::get_timestamp() const
	{
		return std::to_string(time_since_epoch<std::chrono::seconds>());
	}

	std::string coinbase_api::compute_access_sign(std::string_view timestamp, http_verb httpVerb, std::string_view path, std::string_view query, std::string_view body) const
	{
		std::string message;
		message.append(timestamp);
		message.append(to_string(httpVerb));
		message.append(path);
		append_query(message, query);
		message.append(body);
		
		auto hmac = hmac_sha256(message, _decodedApiSecret);
		auto encoded = b64_encode(hmac);
		return encoded;
	}

	exchange_status coinbase_api::get_status() const
	{
		return exchange_status::ONLINE;
	}

	std::vector<tradable_pair> coinbase_api::get_tradable_pairs() const
	{
		return send_public_request<std::vector<tradable_pair>>("/products", coinbase::read_tradable_pairs);
	}

	double coinbase_api::get_price(const tradable_pair& tradablePair) const
	{
		std::string path = "/products/" + tradablePair.to_string(_pairSeparator) + "/ticker";

		return send_public_request<double>(path, coinbase::read_price);
	}

	order_book_state coinbase_api::get_order_book(const tradable_pair& tradablePair, int depth) const
	{
		std::string path = "/products/" + tradablePair.to_string(_pairSeparator) + "/book";

		int level = depth == 1 
			? 1 
			: 2;

		std::string query{ url_query_builder{}
			.add_parameter("level", std::to_string(level))
			.to_string() };

		return send_public_request<order_book_state>(
			path, 
			[depth](std::string_view jsonResult) { return coinbase::read_order_book(jsonResult, depth); },
			query);
	}

	double coinbase_api::get_fee(const tradable_pair& tradablePair) const
	{
		return send_private_request<double>(http_verb::GET, "/fees", coinbase::read_fee);
	}

	unordered_string_map<double> coinbase_api::get_balances() const
	{
		return send_private_request<unordered_string_map<double>>(http_verb::GET, "/coinbase-accounts", coinbase::read_balances);
	}

	std::vector<order_description> coinbase_api::get_open_orders() const
	{
		return send_private_request<std::vector<order_description>>(http_verb::GET, "/orders", coinbase::read_orders);
	}

	std::vector<order_description> coinbase_api::get_closed_orders() const
	{
		std::string query{ url_query_builder{}
			.add_parameter("status", "done")
			.to_string() };

		return send_private_request<std::vector<order_description>>(http_verb::GET, "/orders", coinbase::read_orders, query);
	}

	std::string coinbase_api::add_order(const trade_description& description)
	{
		json_writer json = json_writer{}
			.add("side", to_string(description.action()))
			.add("product_id", description.pair().to_string(_pairSeparator))
			.add("size", std::to_string(description.volume()));

		if (is_stop_order(description.order_type()))
		{
			json.add("type", ::to_string(order_type::LIMIT));
			json.add("price", std::to_string(description.asset_price()));
			json.add("stop", ::to_string(description.order_type()));
			json.add("stop_price", std::to_string(description.asset_price()));
		}
		else
		{
			json.add("type", ::to_string(description.order_type()));

			if (description.order_type() != order_type::MARKET)
			{
				json.add("price", std::to_string(description.asset_price()));
			}
		}

		std::string content{ json.to_string() };

		return send_private_request<std::string>(http_verb::POST, "/orders", coinbase::read_add_order, "", content);
	}

	void coinbase_api::cancel_order(std::string_view orderId)
	{
		std::string path = "/orders/" + std::string{ orderId };

		send_private_request<void>(http_verb::HTTP_DELETE, path, coinbase::read_cancel_order);
	}

	template<>
	std::unique_ptr<exchange> create_exchange_api<coinbase_api>(bool testing)
	{
		return std::make_unique<coinbase_api>(
			internal::load_or_create_config<coinbase_config>(),
			std::make_unique<http_service>(),
			create_exchange_websocket_stream<internal::coinbase_websocket_stream>(),
			testing);
	}
}