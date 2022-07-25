#include "local_directory.h"

#if __APPLE__
#include <mach-o/dyld.h>
#endif

namespace mb
{
#if _WIN32
	std::filesystem::path get_local_directory()
	{
		return "";
	}
#else
    std::filesystem::path get_local_directory()
    {
        std::filesystem::path directory;
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0)
            directory.append(path);
        else
            return "";

        directory.remove_filename();

        return directory;
    }
#endif
}