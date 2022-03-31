#pragma once

#include <string>
#include <spdlog/async.h>

namespace mb
{
	class logger
	{
	private:
		std::shared_ptr<spdlog::logger> _log;

		logger();

	public:
		~logger();

		static logger& instance();

		logger(const logger&) = delete;
		logger(logger&&) noexcept = default;

		logger& operator=(const logger&) = delete;
		logger& operator=(logger&&) noexcept = default;

		template<typename... Args>
		void info(std::string message, Args&&... args)
		{
			_log->info(std::move(message), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void warning(std::string message, Args&&... args)
		{
			_log->warn(std::move(message), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void error(std::string message, Args&&... args)
		{
			_log->error(std::move(message), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void critical(std::string message, Args&&... args)
		{
			_log->critical(std::move(message), std::forward<Args>(args)...);
		}
	};
}