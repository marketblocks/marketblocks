#if _WIN32
#include <windows.h>
#endif

#include <cstring>

#include "time_synchronization.h"
#include "logging/logger.h"

namespace
{
#if _WIN32
	bool sync_time()
	{
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

		LPSTR args = LPSTR("w32tm /resync /nowait");

		if (!CreateProcess(NULL, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
        {
			return false;
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);

		DWORD error;
		GetExitCodeProcess(processInfo.hProcess, &error);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

		return error == 0 ? true : false;
	}
#else
	bool sync_time()
	{
		return false;
	}
#endif

	void run_on_interval()
	{
		using namespace std::chrono_literals;

		mb::logger::instance().info("Syncing local machine time...");

		if (sync_time())
		{
			mb::logger::instance().info("Local machine time synced successfully");
		}
		else
		{
			mb::logger::instance().warning("Machine time could not be synchronized");
		}

		std::this_thread::sleep_for(1024s);
	}
}

namespace mb::internal
{
	time_synchronizer::time_synchronizer()
		: _syncThread{ run_on_interval }
	{}

	time_synchronizer::~time_synchronizer()
	{
		_syncThread.join();
	}
}