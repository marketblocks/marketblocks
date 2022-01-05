#pragma once

#include <string>

#include "common/file/json_wrapper.h"

class KrakenConfig
{
private:
	std::string _publicKey;
	std::string _privateKey;

public:
	KrakenConfig(std::string publicKey, std::string privateKey);

	const std::string& public_key() const { return _publicKey; }
	const std::string& private_key() const { return _privateKey; }

	static KrakenConfig deserialize(JsonWrapper& json);
	std::string serialize() const;
};