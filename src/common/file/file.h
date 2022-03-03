#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace cb
{
	class file_handler
	{
	private:
		std::fstream _stream;

		file_handler(const std::filesystem::path& path, std::ios_base::openmode openMode);

	public:
		static file_handler read(const std::filesystem::path& path);
		static file_handler write(const std::filesystem::path& path);
		static file_handler read_write(const std::filesystem::path& path);

		~file_handler();

		std::fstream& stream() noexcept { return _stream; }
	};

	std::string read_file(const std::filesystem::path& path);
	void write_to_file(const std::filesystem::path& path, std::string_view content);
}