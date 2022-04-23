#include <algorithm>

#include "stringutils.h"

namespace mb
{
	std::vector<std::string> split(std::string_view source, const char delimiter)
	{
		std::stringstream stream{ source.data() };
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(stream, segment, delimiter))
		{
			if (!segment.empty())
				seglist.push_back(segment);
		}

		return seglist;
	}

	void to_upper(std::string& source)
	{
		std::transform(source.begin(), source.end(), source.begin(), ::toupper);
	}

	bool numeric_string_less::operator()(const std::string& l, const std::string& r) const
	{
		double numL = std::stod(l);
		double numR = std::stod(r);

		return numL < numR;
	}

	bool numeric_string_greater::operator()(const std::string& l, const std::string& r) const
	{
		double numL = std::stod(l);
		double numR = std::stod(r);

		return numL > numR;
	}
}