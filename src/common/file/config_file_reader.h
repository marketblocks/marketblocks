#pragma once

#include <string>
#include <filesystem>

#include "common/file/file.h"
#include "common/json/json.h"
#include "logging/logger.h"

namespace cb
{
	std::filesystem::path get_path(std::string_view fileName);
	bool file_exists(std::string_view fileName);

	template<typename Config>
	Config load_config_file()
	{
		std::filesystem::path path{ get_path(Config::name()) };
		std::string jsonString{ read_file(path) };

		return from_json<Config>(jsonString);
	}

	template<typename Config>
	void save_config_file(const Config& config)
	{
		std::filesystem::path path{ get_path(Config::name()) };
		std::string json{ to_json(config).to_string() };
		cb::write_to_file(path, json);
	}

	template<typename Config>
	Config load_or_create_config()
	{
		if (!file_exists(Config::name()))
		{
			logger& log{ logger::instance() };
			log.warning("Config file " + Config::name() + " does not exist, using default values");

			Config config;

			try
			{
				save_config_file(config);
			}
			catch (const std::exception& e)
			{
				log.error("Error occurred saving config file: {}", e.what());
			}

			return config;
		}

		return load_config_file<Config>();
	}
}