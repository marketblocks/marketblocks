#include "json_writer.h"
#include "json.h"

namespace mb
{
	template<>
	json_writer& json_writer::add(std::string_view propertyName, json_document value)
	{
		_document[propertyName.data()] = value._json;

		return *this;
	}

	std::string json_writer::to_string() const
	{
		return _document.dump();
	}

	json_document json_writer::to_json() const
	{
		return json_document{ nlohmann::json{ _document } };
	}
}