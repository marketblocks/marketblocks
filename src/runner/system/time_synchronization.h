#pragma once

#include <thread>

namespace mb::internal
{
	class time_synchronizer
	{
	private:
		std::thread _syncThread;

	public:
		time_synchronizer();
		~time_synchronizer();
	};
}