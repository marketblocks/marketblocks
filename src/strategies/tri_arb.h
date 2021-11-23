#pragma once

#include <memory>
#include <vector>

#include "..\trading.h"
#include "..\exchanges\exchange.h"

class TriArbSequence
{
private:
	const TradeDescription _first;
	const TradeDescription _middle;
	const TradeDescription _last;
	const std::vector<TradablePair> _pairs;

public:
	explicit TriArbSequence(const TradeDescription first, const TradeDescription middle, const TradeDescription last, std::vector<TradablePair>&& pairs)
		: _first{first}, _middle{middle}, _last{last}, _pairs{std::move(pairs)}
	{}

	const TradeDescription& first() const { return _first; }
	const TradeDescription& middle() const { return _middle; }
	const TradeDescription& last() const { return _last; }
	const std::vector<TradablePair>& pairs() const { return _pairs; }
};

class TriArbExchangeSpec
{
private:
	const std::shared_ptr<Exchange> _pexchange;
	const std::vector<TriArbSequence> _sequences;

public:
	explicit TriArbExchangeSpec(std::shared_ptr<Exchange> pexchange, std::vector<TriArbSequence>&& sequences)
		: _pexchange{pexchange}, _sequences{std::move(sequences)}
	{}

	const std::shared_ptr<Exchange> exchange() const { return _pexchange; }
	const std::vector<TriArbSequence>& sequences() const { return _sequences; }
};

class TriArbStrategy
{
private:
	const std::vector<TriArbExchangeSpec> _specs;

	explicit TriArbStrategy(std::vector<TriArbExchangeSpec>&& specs)
		: _specs{specs}
	{}

public:
	static TriArbStrategy create(const std::vector<std::shared_ptr<Exchange>>& exchanges);

	void operator()();
};