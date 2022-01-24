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

		void validate();

	public:
		kraken_config();
		kraken_config(std::string publicKey, std::string privateKey);

		static std::string name() { return "krakenConfig"; }

		const std::string& public_key() const { return _publicKey; }
		const std::string& private_key() const { return _privateKey; }
	};

	template<>
	kraken_config from_json<kraken_config>(const json_document& json);

	template<>
	void to_json<kraken_config>(const kraken_config& config, json_writer& writer);
}