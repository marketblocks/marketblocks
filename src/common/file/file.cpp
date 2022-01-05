#include <fstream>

#include "file.h"

FileHandler::FileHandler(const std::filesystem::path& path, std::ios_base::openmode openMode)
	: _stream{ path, openMode }
{}

FileHandler FileHandler::read(const std::filesystem::path& path)
{
	return FileHandler{ path, std::ios::in };
}

FileHandler FileHandler::write(const std::filesystem::path& path)
{
	return FileHandler{ path, std::ios::out };
}

FileHandler FileHandler::read_write(const std::filesystem::path& path)
{
	return FileHandler{ path, std::ios::in | std::ios::out };
}

FileHandler::~FileHandler()
{
	_stream.close();
}

std::string read_file(const std::filesystem::path& path)
{
	FileHandler handler = FileHandler::read(path);
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