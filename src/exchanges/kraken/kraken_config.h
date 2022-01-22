#pragma once

#include <string>

#include "common/file/json.h"

namespace cb
{
	class kraken_config
	{
	private:
		std::string _publicKey;
		std::string _privateKey;

	public:
		kraken_config(std::string publicKey, std::string privateKey);

		static kraken_config create_default();
		static std::string name() { return "krakenConfig"; }
		static kraken_config deserialize(json_document& json);
		std::string serialize() const;

		const std::string& public_key() const { return _publicKey; }
		const std::string& private_key() const { return _privateKey; }
	};
}