#include "stringutils.h"

namespace cb
{
	std::vector<std::string> split(const std::string& source, const char delimiter)
	{
		std::stringstream stream{ source };
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(stream, segment, delimiter))
		{
			if (!segment.empty())
				seglist.push_back(segment);
		}

		return seglist;
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