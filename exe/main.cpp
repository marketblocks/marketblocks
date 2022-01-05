#include <iostream>

#include "runner/runner.h"
#include "strategies/tri_arb.h"

int main()
{
	Runner<TriArbStrategy> runner {RunMode::LIVETEST};
	runner.initialise();
	runner.run();

	return 0;
}
