#include "live_test_runner.h"
#include "common/utils/containerutils.h"

namespace mb::internal
{
	test_logger create_test_logger(const std::vector<std::shared_ptr<live_test_exchange>>& liveTestExchanges)
	{
		auto paperTradeApis = to_vector<std::shared_ptr<paper_trade_api>>(liveTestExchanges,
			[](std::shared_ptr<live_test_exchange> liveTestExchange) { return liveTestExchange->trade_api(); });

		return create_test_logger(std::move(paperTradeApis));
	}
}