#include "config_file_readers.h"
#include "common/file/file.h"
#include "common/file/json_wrapper.h"

namespace
{
	const std::filesystem::path config_directory() { return "C:\\Users\\jorda\\Documents"; }

	namespace file_names
	{
		const std::string runner_config() { return "runnerConfig"; }
		const std::string trading_options() { return "tradingOptions"; }
		const std::string kraken_config() {	return "krakenConfig"; }
	}

	std::filesystem::path get_path(const std::string& fileName)
	{
		std::filesystem::path path = config_directory();
		path /= fileName + ".json";

		return path;
	}

	template<typename JsonObject>
	JsonObject load_config_file(const std::string& fileName)
	{
		std::filesystem::path path = get_path(fileName);

		std::string json = cb::read_file(path);
		cb::json_wrapper wrapper { json };

		return JsonObject::deserialize(wrapper);
	}

	template<typename JsonObject>
	void save_config_file(const std::string& fileName, const JsonObject& jsonObject)
	{
		std::filesystem::path path = get_path(fileName);
		std::string json = jsonObject.serialize();
		cb::write_to_file(path, json);
	}
}

namespace cb
{
	runner_config load_runner_config()
	{
		return load_config_file<runner_config>(file_names::runner_config());
	}

	void save_runner_config(const runner_config& config)
	{
		save_config_file(file_names::runner_config(), config);
	}

	trading_options load_trading_options()
	{
		return load_config_file<trading_options>(file_names::trading_options());
	}

	void save_trading_options(const trading_options& options)
	{
		save_config_file(file_names::trading_options(), options);
	}

	kraken_config load_kraken_config()
	{
		return load_config_file<kraken_config>(file_names::kraken_config());
	}

	void save_kraken_config(const kraken_config& config)
	{
		save_config_file(file_names::kraken_config(), config);
	}
}