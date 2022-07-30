#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace mb
{
	std::vector<std::string> split(std::string_view source, const char delimiter);
	void to_upper(std::string& source);
	void to_lower(std::string& source);

	template<typename String>
	bool contains(const std::string& source, const String& what)
	{
		return source.find(what) != std::string::npos;
	}

	struct numeric_string_less
	{
		bool operator()(const std::string& l, const std::string& r) const;
	};

	struct numeric_string_greater
	{
		bool operator()(const std::string& l, const std::string& r) const;
	};
}