#include <iostream>

#include "runner/runner.h"
#include "strategies/tri_arb.h"

int main()
{
	Runner<TriArbStrategy> runner;
	runner.initialise();
	runner.run();

	return 0;
}
