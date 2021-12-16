#pragma once

#include <string>

class HttpResponse
{
private:
	int _responseCode;
	std::string _message;

public:
	HttpResponse(int responseCode, std::string message);

	int response_code() const { return _responseCode; }
	const std::string& message() const { return _message; }
};