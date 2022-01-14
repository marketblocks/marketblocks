#pragma once

#include <rapidjson/document.h>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace cb
{
	class json_wrapper
	{
	private:
		rapidjson::Document _document;

	public:
		explicit json_wrapper(const std::string_view& json);

		rapidjson::Document& document() { return _document; }

		std::vector<std::string> get_string_array(const std::string_view& id) const;
	};
}