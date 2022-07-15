#pragma once

#include "common/json/json.h"

namespace mb
{
	class template_config
	{
	private:
		std::string _publicKey;
		std::string _privateKey;

	public:
		constexpr template_config()
			: _publicKey{}, _privateKey{}
		{};

		constexpr template_config(std::string publicKey, std::string privateKey)
			: _publicKey{ std::move(publicKey) }, _privateKey{ std::move(privateKey) }
		{}

		static constexpr std::string name() noexcept { return "template"; }

		constexpr const std::string& public_key() const noexcept { return _publicKey; }
		constexpr const std::string& private_key() const noexcept { return _privateKey; }
	};

	template<>
	template_config from_json<template_config>(const json_document& json);

	template<>
	void to_json<template_config>(const template_config& config, json_writer& writer);
}