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

	std::unique_ptr<internal::time_synchronizer> create_synchronizer_if_enabled(run_mode runMode, bool enabled)
	{
		if (enabled && (runMode == run_mode::LIVE || runMode == run_mode::LIVETEST))
		{
			return std::make_unique<internal::time_synchronizer>();
		}

		return nullptr;
	}
}