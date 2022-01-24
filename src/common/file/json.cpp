#include "json.h"

namespace cb
{
	json_document parse_json(const std::string& jsonString)
	{
		return json_document{ nlohmann::json::parse(jsonString) };
	}
}