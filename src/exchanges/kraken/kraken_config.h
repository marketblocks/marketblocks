#pragma once

#include <string>

#include "common/file/json_wrapper.h"

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

		const std::string& public_key() const { return _publicKey; }
		const std::string& private_key() const { return _privateKey; }

		static kraken_config deserialize(json_wrapper& json);
		std::string serialize() const;
	};
}