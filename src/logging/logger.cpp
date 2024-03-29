#include "logger.h"
#include "common/file/local_directory.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

namespace
{
	using namespace mb;

	std::shared_ptr<spdlog::logger> init_logger()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		std::filesystem::path logPath{ get_local_directory() };
		logPath /= "log.txt";
		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath.string(), true);
		fileSink->set_level(spdlog::level::err);

		constexpr int QUEUE_SIZE = 8192;
		constexpr int THREAD_COUNT = 1;
		spdlog::init_thread_pool(QUEUE_SIZE, THREAD_COUNT);

		auto logger = std::make_shared<spdlog::async_logger>(
			"default", 
			spdlog::sinks_init_list{ consoleSink, fileSink },
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		spdlog::register_logger(logger);
		spdlog::set_pattern("[%d/%m/%Y %H:%M:%S] [%^%l%$] %v");

		return logger;
	}
}

namespace mb
{
	logger::logger()
		: _log{ init_logger() }
	{}

	logger::~logger()
	{
		spdlog::shutdown();
	}

	logger& logger::instance()
	{
		static logger instance;
		return instance;
	}
}