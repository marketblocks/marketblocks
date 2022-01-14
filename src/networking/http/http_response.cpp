#include "http_response.h"

namespace cb
{
	http_response::http_response(int responseCode, std::string message)
		: _responseCode{ responseCode }, _message{ std::move(message) }
	{}
}