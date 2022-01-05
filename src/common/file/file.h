#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

class FileHandler
{
private:
	std::fstream _stream;

	FileHandler(const std::filesystem::path& path, std::ios_base::openmode openMode);

public:
	static FileHandler read(const std::filesystem::path& path);
	static FileHandler write(const std::filesystem::path& path);
	static FileHandler read_write(const std::filesystem::path& path);

	~FileHandler();

	std::fstream& stream() { return _stream; }
};

std::string read_file(const std::filesystem::path& path);

void write_to_file(const std::filesystem::path& path, const std::string& content);