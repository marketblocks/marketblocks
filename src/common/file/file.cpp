#include <fstream>

#include "file.h"

namespace cb 
{
	file_handler::file_handler(const std::filesystem::path& path, std::ios_base::openmode openMode)
		: _stream{ path, openMode }
	{}

	file_handler file_handler::read(const std::filesystem::path& path)
	{
		return file_handler{ path, std::ios::in };
	}

	file_handler file_handler::write(const std::filesystem::path& path)
	{
		return file_handler{ path, std::ios::out };
	}

	file_handler file_handler::read_write(const std::filesystem::path& path)
	{
		return file_handler{ path, std::ios::in | std::ios::out };
	}

	file_handler::~file_handler()
	{
		_stream.close();
	}

	std::string read_file(const std::filesystem::path& path)
	{
		file_handler handler = file_handler::read(path);
		std::string contents{ (std::istreambuf_iterator<char>(handler.stream())), std::istreambuf_iterator<char>() };

		if (handler.stream().good())
		{
			return contents;
		}

		// return error
		return "";
	}

	void write_to_file(const std::filesystem::path& path, const std::string& content)
	{

	}
}