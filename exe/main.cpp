#include <iostream>

#include "runner/runner.h"
#include "strategies/tri_arb.h"

int main()
{
	cb::runner<tri_arb_strategy> runner { cb::run_mode::LIVETEST };
	runner.initialise();
	runner.run();

	return 0;
}
