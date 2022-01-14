#include "http_header.h"

namespace cb
{
	http_header::http_header(std::string key, std::string value)
		: _key{ std::move(key) }, _value{ std::move(value) }
	{}

	std::string http_header::to_string() const
	{
		return _key + ": " + _value;
	}
}