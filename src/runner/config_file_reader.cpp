#include "config_file_reader.h"

namespace
{
	constexpr std::string_view CONFIG_DIRECTORY = "configs";
}

namespace mb
{
	namespace internal
	{
		std::filesystem::path get_path(std::string_view fileName)
		{
			std::filesystem::path path{ CONFIG_DIRECTORY };
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
			std::filesystem::path path{ CONFIG_DIRECTORY };
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
		}
	}
}