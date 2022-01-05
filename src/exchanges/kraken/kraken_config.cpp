#include "kraken_config.h"

KrakenConfig::KrakenConfig(std::string publicKey, std::string privateKey)
	: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
{}

KrakenConfig KrakenConfig::deserialize(JsonWrapper& json)
{
	std::string publicKey = json.document()["publicKey"].GetString();
	std::string privateKey = json.document()["privateKey"].GetString();

	return KrakenConfig{ std::move(publicKey), std::move(privateKey) };
}

std::string KrakenConfig::serialize() const
{
	return "";
}