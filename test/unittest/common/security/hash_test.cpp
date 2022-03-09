#include <gtest/gtest.h>

#include "common/security/hash.h"

namespace
{
	std::string convert_to_plain_text(const std::vector<unsigned char>& data)
	{
		constexpr std::streamsize width = 2;
		constexpr char fill = '0';

		std::stringstream stream;
		for (int i = 0; i < data.size(); i++)
		{
			stream << std::hex << std::setw(width) << std::setfill(fill) << (int)data[i];
		}

		return stream.str();
	}
}

namespace cb::test
{
	/*TEST(Hash, ComputesCorrectSHA256)
	{
		EXPECT_EQ(convert_to_plain_text(sha256("This is a test string")), "717ac506950da0ccb6404cdd5e7591f72018a20cbca27c8a423e9c9e5626ac61");
		EXPECT_EQ(convert_to_plain_text(sha256("Another test string")), "15f82fb11e8578f6faccffbcfacec21efb352f6796602e2ab1ef719c15324436");
	}

	TEST(Hash, ComputesCorrectSHA512)
	{
		EXPECT_EQ(convert_to_plain_text(sha512("This is a test string")), "b8ee69b29956b0b56e26d0a25c6a80713c858cf2902a12962aad08d682345646b2d5f193bbe03997543a9285e5932f34baf2c85c89459f25ba1cf43c4410793c");
		EXPECT_EQ(convert_to_plain_text(sha512("Another test string")), "d925c8ed1a6ec283537efac85997f3783d3fd70dda9fac98f49fbfd1d28090abc523052b0b2e206ababaff19e44254767526c493965a060602ead26c8a5d9f55");
	}*/
}