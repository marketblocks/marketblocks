#pragma once

#include <string_view>
#include <nlohmann/json.hpp>

namespace mb
{
	static constexpr std::string_view JSON_FILE_EXTENSION = "json";

	template<typename T>
	class json;

	typedef json<nlohmann::json> json_document;
	typedef json<const nlohmann::json&> json_element;

	enum class json_value_type
	{
		OBJECT,
		ARRAY,
		INT,
		DOUBLE,
		BOOL,
		STRING,
		UNKNOWN
	};
}