#include "runner/runner.h"
#include "logging/logger.h"
#include "strategies/tri_arb.h"

int main()
{
	cb::runner<tri_arb_strategy> runner { cb::run_mode::LIVETEST };
	
	try
	{
		runner.initialise();
		runner.run();
	}
	catch (const std::exception& e)
	{
		cb::logger::instance().critical(e.what());
		abort();
	}

	return 0;
}
