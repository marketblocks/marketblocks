#if __APPLE__
#include <mach-o/dyld.h>
#endif

#include "config_file_reader.h"

namespace
{
	constexpr std::string_view CONFIG_DIRECTORY = "configs";

#if _WIN32
    std::filesystem::path get_config_directory()
    {
        return CONFIG_DIRECTORY;
    }
#else
    std::filesystem::path get_config_directory()
    {
        std::filesystem::path directory;
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0)
            directory.append(path);
        else
            return "";
        
        directory.remove_filename();
        directory.append(CONFIG_DIRECTORY);
        return directory;
    }
#endif
}

namespace mb
{
	namespace internal
	{
		std::filesystem::path get_path(std::string_view fileName)
		{
			std::filesystem::path path{ get_config_directory() };
			path.append(fileName);
			path.replace_extension(JSON_FILE_EXTENSION);

			return path;
		}

		bool file_exists(std::string_view fileName)
		{
			return std::filesystem::exists(get_path(fileName));
		}

		void create_config_directory_if_not_exist()
		{
			std::filesystem::path path{ get_config_directory() };
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
		}
	}
}
