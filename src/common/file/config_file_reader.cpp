#include "config_file_reader.h"

namespace
{
	const std::filesystem::path config_directory() { return "C:\\Users\\jorda\\Documents"; }
}

namespace cb
{
	std::filesystem::path get_path(const std::string& fileName)
	{
		std::filesystem::path path = config_directory();
		path /= fileName + ".json";

		return path;
	}

	bool file_exists(const std::string& fileName)
	{
		return std::filesystem::exists(get_path(fileName));
	}
}