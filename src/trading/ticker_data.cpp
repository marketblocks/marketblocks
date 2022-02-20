#include "ticker_data.h"

namespace cb
{
	ticker_data::ticker_data(
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
		_bidVolume { bidVolume },
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
}

