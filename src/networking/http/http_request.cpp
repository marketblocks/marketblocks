#include "http_request.h"

namespace mb
{
	http_request::http_request(http_verb verb, std::string uri)
		: _verb{ verb }, _url{ std::move(uri) }, _content{}, _headers{}
	{}

	void http_request::add_header(std::string_view key, std::string_view value)
	{
		_headers.emplace_back(std::string{ key }, std::string{ value });
	}

	std::ostream& operator<<(std::ostream& stream, const http_request& request)
	{
		stream << "Verb: " << to_string(request.verb()) << std::endl;
		stream << "Url: " << request.url() << std::endl;
		stream << "Content: " << request.content() << std::endl;

		return stream;
	}
}