#include "http_response.h"

HttpResponse::HttpResponse(int responseCode, std::string message)
	: _responseCode{ responseCode }, _message{ std::move(message) }
{}