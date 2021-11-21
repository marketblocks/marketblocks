#include <vector>

#include "..\data.h"
#include "..\trading.h"

namespace CryptoBot
{
	class Exchange
	{
		virtual ~Exchange() = default;

		virtual double get_fee() const = 0;
		virtual PriceData get_price_data(const std::vector<TradablePair>& tradablePairs) const = 0;
	};
}