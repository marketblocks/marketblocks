#include "json.h"

namespace cb
{
	json_document parse_json(const std::string& jsonString)
	{
		return json_document{ value_proxy<const nlohmann::json>{ nlohmann::json::parse(jsonString) } };
	}
}