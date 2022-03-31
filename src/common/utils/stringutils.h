#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace mb
{
	std::vector<std::string> split(std::string_view source, const char delimiter);

	struct numeric_string_less
	{
		bool operator()(const std::string& l, const std::string& r) const;
	};

	struct numeric_string_greater
	{
		bool operator()(const std::string& l, const std::string& r) const;
	};
}