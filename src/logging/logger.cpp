#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

namespace
{
	std::shared_ptr<spdlog::logger> init_logger()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		//auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
		
		constexpr int QUEUE_SIZE = 8192;
		constexpr int THREAD_COUNT = 1;
		spdlog::init_thread_pool(QUEUE_SIZE, THREAD_COUNT);

		auto logger = std::make_shared<spdlog::async_logger>(
			"default", 
			spdlog::sinks_init_list{ consoleSink },
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		spdlog::register_logger(logger);
		spdlog::set_pattern("[%d/%m/%Y %H:%M:%S] [%^%l%$] %v");

		return logger;
	}
}

namespace cb
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