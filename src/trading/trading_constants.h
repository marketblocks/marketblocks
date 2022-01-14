#pragma once

namespace cb
{
	enum class trade_action
	{
		SELL = 0,
		BUY = 1
	};

	enum class trade_result
	{
		SUCCESS = 0,

		INSUFFICENT_FUNDS,
	};
}