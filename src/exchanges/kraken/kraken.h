#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "kraken_config.h"
#include "kraken_websocket.h"
#include "exchanges/exchange.h"
#include "networking/http/http_service.h"
#include "networking/websocket/websocket_connection.h"
#include "trading/trade_description.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace cb
{
	namespace internal
	{
		struct kraken_constants
		{
			inline static const std::string BASEURL = "https://api.kraken.com";
			inline static const std::string VERSION = "0";
			inline static const std::string PUBLIC = "public";
			inline static const std::string PRIVATE = "private";

			inline static const std::string TRADABLE_PAIRS = "AssetPairs";
			inline static const std::string ORDER_BOOK = "Depth";
			inline static const std::string BALANCE = "Balance";
		};
	}
	
	class kraken_api final : public exchange
	{
	private:
		internal::kraken_constants _constants;
		
		std::string _publicKey;
		std::vector<unsigned char> _decodedPrivateKey;
		http_service _httpService;
		kraken_websocket_stream _websocketStream;
		std::shared_ptr<websocket_client> _websocketClient;

		std::string build_url_path(const std::string& access, const std::string& method) const;
		std::string build_kraken_url(const std::string& access, const std::string& method, const std::string& query) const;

		std::string get_nonce() const;
		std::string compute_api_sign(const std::string& uriPath, const std::string& postData, const std::string& nonce) const;

		std::string send_public_request(const std::string& method, const std::string& query) const;
		std::string send_public_request(const std::string& method) const;

		std::string send_private_request(const std::string& method, const std::string& query) const;
		std::string send_private_request(const std::string& method) const;

	public:
		kraken_api(kraken_config config, http_service httpService, std::shared_ptr<websocket_client> websocketClient);

		const std::vector<tradable_pair> get_tradable_pairs() const override;
		const std::unordered_map<tradable_pair, order_book_state> get_order_book(const std::vector<tradable_pair>& tradablePairs, int depth) const override;
		const std::unordered_map<tradable_pair, double> get_fees(const std::vector<tradable_pair>& tradablePairs) const override;
		const std::unordered_map<asset_symbol, double> get_balances() const override;
		trade_result trade(const trade_description& description) override;

		websocket_stream& get_or_connect_websocket() override;
	};
}