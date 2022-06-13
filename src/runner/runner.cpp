#include "runner.h"
#include "project_settings.h"

namespace mb::internal
{
	void log_version()
	{
		logger::instance().info("{0} v{1}", PROJECT_NAME, PROJECT_VERSION);
	}

	void log_run_mode(run_mode runMode)
	{
		logger::instance().info("Running {} mode", to_string(runMode));
	}

	void exit_on_key()
	{
		logger::instance().info("\nPress Enter to exit...");
		std::cin.get();
		std::exit(-1);
	}
}