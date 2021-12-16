#include "http_header.h"

HttpHeader::HttpHeader(std::string key, std::string value)
	: _key{ std::move(key) }, _value{ std::move(value) }
{}

std::string HttpHeader::to_string() const
{
	return _key + ": " + _value;
}