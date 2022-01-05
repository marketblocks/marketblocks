#pragma once

#include <rapidjson/document.h>
#include <string_view>
#include <vector>
#include <unordered_map>

class JsonWrapper
{
private:
	rapidjson::Document _document;

public:
	explicit JsonWrapper(const std::string_view& json);

	rapidjson::Document& document() { return _document; }

	std::vector<std::string> get_string_array(const std::string_view& id) const;
};