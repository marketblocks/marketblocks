#include <vector>
#include <string>

#include <vector>
#include <string>

namespace cb
{
	std::string b64_encode(const std::vector<unsigned char>& data);
	std::vector<unsigned char> b64_decode(std::string_view data);
}
