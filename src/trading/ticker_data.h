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
		constexpr ticker_data(
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
			double openingPrice)
			:
			_askPrice{ askPrice },
			_askVolume{ askVolume },
			_bidPrice{ bidPrice },
			_bidVolume{ bidVolume },
			_volumeToday{ volumeToday },
			_volume24{ volume24 },
			_tradesToday{ tradesToday },
			_trades24{ trades24 },
			_lowToday{ lowToday },
			_low24{ low24 },
			_highToday{ highToday },
			_high24{ high24 },
			_openingPrice{ openingPrice }
		{}

		constexpr double ask_price() const noexcept { return _askPrice; }
		constexpr double ask_volume() const noexcept { return _askVolume; }
		constexpr double bid_price() const noexcept { return _bidPrice; }
		constexpr double bid_volume() const noexcept { return _bidVolume; }
		constexpr double volume_today() const noexcept { return _volumeToday; }
		constexpr double volume_24() const noexcept { return _volume24; }
		constexpr int trades_today() const noexcept { return _tradesToday; }
		constexpr int trades_24() const noexcept { return _trades24; }
		constexpr double low_today() const noexcept { return _lowToday; }
		constexpr double low_24() const noexcept { return _low24; }
		constexpr double high_today() const noexcept { return _highToday; }
		constexpr double high_24() const noexcept { return _high24; }
		constexpr double opening_price() const noexcept { return _openingPrice; }
	};
}
