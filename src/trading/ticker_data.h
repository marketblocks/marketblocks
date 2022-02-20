#pragma once

namespace cb
{
	class ticker_data
	{
	private:
		double _askPrice;
		double _askVolume;
		double _bidPrice;
		double _bidVolume;
		double _volumeToday;
		double _volume24;
		int _tradesToday;
		int _trades24;
		double _lowToday;
		double _low24;
		double _highToday;
		double _high24;
		double _openingPrice;

	public:
		ticker_data(
			double askPrice,
			double askVolume,
			double bidPrice,
			double bidVolume,
			double volumeToday,
			double volume24,
			int tradesToday,
			int trades24,
			double lowToday,
			double low24,
			double highToday,
			double high24,
			double openingPrice);

		double ask_price() const { return _askPrice; }
		double ask_volume() const { return _askVolume; }
		double bid_price() const { return _bidPrice; }
		double bid_volume() const { return _bidVolume; }
		double volume_today() const { return _volumeToday; }
		double volume_24() const { return _volume24; }
		int trades_today() const { return _tradesToday; }
		int trades_24() const { return _trades24; }
		double low_today() const { return _lowToday; }
		double low_24() const { return _low24; }
		double high_today() const { return _highToday; }
		double high_24() const { return _high24; }
		double opening_price() const { return _openingPrice; }
	};
}
