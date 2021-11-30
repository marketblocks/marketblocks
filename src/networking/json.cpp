#include "json.h"

JsonWrapper::JsonWrapper(const std::string_view& json)
	: _document{}
{
	_document.Parse(json.data());
}

std::vector<std::string> JsonWrapper::get_string_array(const std::string_view& id) const
{
	auto jsonArray = _document[id.data()].GetArray();
	std::vector<std::string> result;
	result.reserve(jsonArray.Size());

	for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it)
	{
		result.push_back(it->GetString());
	}

	return result;
}