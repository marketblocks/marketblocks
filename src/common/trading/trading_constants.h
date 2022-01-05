#pragma once

enum class TradeAction
{
	SELL = 0,
	BUY = 1
};

enum class TradeResult
{
	SUCCESS = 0,

	INSUFFICENT_FUNDS,
};