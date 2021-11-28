#pragma once

namespace CryptoBot
{
	namespace runner
	{
		template<typename Strategy>
		void run(Strategy s)
		{
			while (true)
			{
				s();
			}
		}
	}
}