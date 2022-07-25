#pragma once

#include <stdexcept>
#include <string>

namespace mb
{
	class mb_exception : public std::exception
	{
	private:
		std::string _message;

	public:
		mb_exception(std::string message)
			: _message{ std::move(message) }
		{}

		const char* what() const noexcept override { return _message.c_str(); }
	};
}