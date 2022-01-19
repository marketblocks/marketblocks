#include <iostream>
#include <cassert>

#include "runner/runner.h"
#include "logging/logger.h"
#include "strategies/tri_arb.h"

int main()
{
	cb::runner<tri_arb_strategy> runner { cb::run_mode::LIVETEST };
	
	try
	{
		runner.initialise();
	}
	catch (const cb::initialisation_error& e)
	{
		cb::logger::instance().critical("Initialisation failed: {}", e.what());
		abort();
	}

	runner.run();
	return 0;
}
