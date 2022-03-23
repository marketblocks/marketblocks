#pragma once

#include <nlohmann/json.hpp>

#include "json_constants.h"

namespace cb
{
	class json_writer
	{
	private:
		nlohmann::json _document;

	public:
		template<typename T>
		json_writer& add(std::string_view propertyName, T value)
		{
			_document[propertyName.data()] = std::move(value);

			return *this;
		}

		template<>
		json_writer& add(std::string_view propertyName, json_document value);

		std::string to_string() const;

		json_document to_json() const;
	};
}
