#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace mb
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

		file_handler(file_handler&& other) noexcept;
		file_handler& operator=(file_handler&& other) noexcept;

		std::fstream& stream() noexcept { return _stream; }
	};

	std::string read_file(const std::filesystem::path& path);
	void write_to_file(const std::filesystem::path& path, std::string_view content);

	template<typename OnNewLine>
	void stream_file(const std::filesystem::path& path, OnNewLine onNewLine)
	{
		file_handler handler{ file_handler::read(path) };
		std::string line;

		while (std::getline(handler.stream(), line))
		{
			onNewLine(line);
		}
	}
}