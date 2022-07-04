#pragma once

#include "common/json/json.h"

namespace mb
{
	class dextrade_config
	{
	private:
		std::string _publicKey;
		std::string _privateKey;

	public:
		constexpr dextrade_config()
			: _publicKey{}, _privateKey{}
		{};

		constexpr dextrade_config(std::string publicKey, std::string privateKey)
			: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
		{}

		static constexpr std::string name() noexcept { return "dextrade"; }

		constexpr const std::string& public_key() const noexcept { return _publicKey; }
		constexpr const std::string& private_key() const noexcept { return _privateKey; }
	};

	template<>
	dextrade_config from_json<dextrade_config>(const json_document& json);

	template<>
	void to_json<dextrade_config>(const dextrade_config& config, json_writer& writer);
}