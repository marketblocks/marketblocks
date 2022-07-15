#include "template_config.h"

namespace
{
	namespace json_property_names
	{
		static constexpr std::string_view PUBLIC_KEY = "publicKey";
		static constexpr std::string_view PRIVATE_KEY = "privateKey";
	}
}

namespace mb
{
	template<>
	template_config from_json<template_config>(const json_document& json)
	{
		return template_config
		{
			json.get<std::string>(json_property_names::PUBLIC_KEY),
			json.get<std::string>(json_property_names::PRIVATE_KEY)
		};
	}

	template<>
	void to_json<template_config>(const template_config& config, json_writer& writer)
	{
		writer.add(json_property_names::PUBLIC_KEY, config.public_key());
		writer.add(json_property_names::PRIVATE_KEY, config.private_key());
	}
}