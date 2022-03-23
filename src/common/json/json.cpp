#include "json.h"

namespace cb
{
	json_document parse_json(std::string_view jsonString)
	{
		return json_document{ nlohmann::json::parse(jsonString) };
	}
}