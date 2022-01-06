#include "stringutils.h"

std::vector<std::string> split(const std::string& source, const char delimiter)
{
	std::stringstream stream{ source };
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(stream, segment, delimiter))
	{
		seglist.push_back(segment);
	}

	return seglist;
}