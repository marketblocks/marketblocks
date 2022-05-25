#pragma once

#include <string>
#include <filesystem>

#include "common/file/file.h"
#include "common/json/json.h"
#include "logging/logger.h"

namespace mb
{
	namespace internal
	{
		std::filesystem::path get_path(std::string_view fileName);
		bool file_exists(std::string_view fileName);
		void create_config_directory_if_not_exist();

		template<typename Config>
		Config load_config_file_or_default()
		{
			std::filesystem::path path{ get_path(Config::name()) };

			try
			{
				logger::instance().info("Reading config file: {}", Config::name());
				std::string jsonString{ read_file(path) };
				return from_json<Config>(jsonString);
			}
			catch (const std::exception& e)
			{
				logger::instance().error("Error occurred reading {0}: {1}, using default values", Config::name(), e.what());
				return Config{};
			}
		}

		template<typename Config>
		void save_config_file(const Config& config)
		{
			std::filesystem::path path{ get_path(Config::name()) };
			std::string json{ to_json(config).to_string() };

			try
			{
				write_to_file(path, json);
			}
			catch (const std::exception& e)
			{
				logger::instance().error("Error occurred saving config file: {}", e.what());
			}
		}
	}

	template<typename Config>
	Config load_or_create_config()
	{
		if (!internal::file_exists(Config::name()))
		{
			logger::instance().warning("Config file " + Config::name() + " does not exist, using default values");

			Config config;
			internal::save_config_file(config);
			
			return config;
		}

		return internal::load_config_file_or_default<Config>();
	}
}