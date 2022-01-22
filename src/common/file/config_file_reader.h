#pragma once

#include <string>
#include <filesystem>

#include "common/file/file.h"
#include "common/file/json.h"
#include "logging/logger.h"

namespace cb
{
	std::filesystem::path get_path(const std::string& fileName);

	bool file_exists(const std::string& fileName);

	template<typename Config>
	Config load_config_file()
	{
		std::filesystem::path path = get_path(Config::name());
		std::string jsonString = cb::read_file(path);
		json_document jsonDocument = parse_json(jsonString);

		return Config::deserialize(jsonDocument);
	}

	template<typename Config>
	void save_config_file(const Config& jsonObject)
	{
		std::filesystem::path path = get_path(Config::name());
		std::string json = jsonObject.serialize();
		cb::write_to_file(path, json);
	}

	template<typename Config>
	Config load_or_create_config()
	{
		if (!file_exists(Config::name()))
		{
			logger::instance().warning("Config file " + Config::name() + "does not exist, using default values");

			Config config = Config::create_default();
			save_config_file(config);

			return config;
		}

		return load_config_file<Config>();
	}
}