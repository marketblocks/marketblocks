#include "config_file_readers.h"
#include "common/file/file.h"
#include "common/file/json_wrapper.h"

namespace
{
	namespace file_names
	{
		const std::string RUNNER_CONFIG = "runnerConfig";
		const std::string TRADING_OPTIONS = "tradingOptions";
		const std::string KRAKEN_CONFIG = "krakenConfig";
	}

	static const std::filesystem::path configDirectory = "C:\\Users\\jorda\\Documents";

	std::filesystem::path get_path(const std::string& fileName)
	{
		std::filesystem::path path = configDirectory;
		path /= fileName + ".json";

		return path;
	}

	template<typename JsonObject>
	JsonObject load_config_file(const std::string& fileName)
	{
		std::filesystem::path path = get_path(fileName);

		std::string json = read_file(path);
		JsonWrapper wrapper { json };

		return JsonObject::deserialize(wrapper);
	}

	template<typename JsonObject>
	void save_config_file(const std::string& fileName, const JsonObject& jsonObject)
	{
		std::filesystem::path path = get_path(fileName);
		std::string json = jsonObject.serialize();
		write_to_file(path, json);
	}
}

RunnerConfig load_runner_config()
{
	return load_config_file<RunnerConfig>(file_names::RUNNER_CONFIG);
}

void save_runner_config(const RunnerConfig& config)
{
	save_config_file(file_names::RUNNER_CONFIG, config);
}

TradingOptions load_trading_options()
{
	return load_config_file<TradingOptions>(file_names::TRADING_OPTIONS);
}

void save_trading_options(const TradingOptions& options)
{
	save_config_file(file_names::TRADING_OPTIONS, options);
}

KrakenConfig load_kraken_config()
{
	return load_config_file<KrakenConfig>(file_names::KRAKEN_CONFIG);
}

void save_kraken_config(const KrakenConfig& config)
{
	save_config_file(file_names::KRAKEN_CONFIG, config);
}