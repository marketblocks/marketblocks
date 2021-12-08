#pragma once

#include "common/trading/trading_constants.h"
#include "common/data/data.h"

double calculate_cost(double assetPrice, double volume);

double calculate_volume(double assetPrice, double cost);

double calculate_asset_price(double cost, double volume);

double calculate_fee(double amount, double feePercentage);

double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, TradeAction action);

double select_price(const PriceData& prices, TradeAction action);