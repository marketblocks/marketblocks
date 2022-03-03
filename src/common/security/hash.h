#pragma once

#include <string>
#include <vector>

namespace cb
{
	std::vector<unsigned char> sha256(std::string_view data);
	std::vector<unsigned char> sha512(std::string_view data);

	std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key);

	std::string convert_to_plain_text(const std::vector<unsigned char>& data);
}