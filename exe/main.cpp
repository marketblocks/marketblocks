#include <iostream>
#include <cassert>

#include "runner/runner.h"
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
		std::cerr << "Initialisation failed: " << e.what() << std::endl;
		abort();
	}

	runner.run();
	return 0;
}
