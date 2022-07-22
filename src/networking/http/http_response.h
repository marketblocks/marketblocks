#pragma once

#include <string>

namespace mb
{
	class http_response
	{
	private:
		int _responseCode;
		std::string _message;

	public:
		http_response(int responseCode, std::string message)
			: _responseCode{ responseCode }, _message{ std::move(message) }
		{}

		int response_code() const noexcept { return _responseCode; }
		const std::string& message() const noexcept { return _message; }
	};
}