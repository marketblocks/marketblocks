#pragma once

#include <string>

namespace cb
{
	class http_response
	{
	private:
		int _responseCode;
		std::string _message;

	public:
		http_response(int responseCode, std::string message);

		int response_code() const { return _responseCode; }
		const std::string& message() const { return _message; }
	};
}