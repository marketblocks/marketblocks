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
		constexpr http_response(int responseCode, std::string message)
			: _responseCode{ responseCode }, _message{ std::move(message) }
		{}

		constexpr int response_code() const noexcept { return _responseCode; }
		constexpr const std::string& message() const noexcept { return _message; }
	};
}