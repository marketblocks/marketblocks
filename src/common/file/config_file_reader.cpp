#include "config_file_reader.h"
#include "local_directory.h"

namespace
{
	using namespace mb;

	constexpr std::string_view CONFIG_DIRECTORY = "configs";

    std::filesystem::path get_config_directory()
    {
		std::filesystem::path directory{ get_local_directory() };
        directory.append(CONFIG_DIRECTORY);

		return directory;
    }
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
