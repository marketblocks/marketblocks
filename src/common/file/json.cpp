#include "json.h"

namespace cb
{
	json_document parse_json(const std::string& jsonString)
	{
		json_document proxy{ nlohmann::json::parse(jsonString) };
		return json_document{ std::move(proxy) };
	}
}