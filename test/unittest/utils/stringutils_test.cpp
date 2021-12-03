#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/stringutils.h"

using testing::ElementsAre;
using testing::IsEmpty;

TEST(StringUtils, SplitValidArgs)
{
	EXPECT_THAT(split("This_is_underscore_test", '_'), ElementsAre("This", "is", "underscore", "test"));
	EXPECT_THAT(split("This-is-hyphen-test", '-'), ElementsAre("This", "is", "hyphen", "test"));
	EXPECT_THAT(split("This,is,comma,test", ','), ElementsAre("This", "is", "comma", "test"));
}

TEST(StringUtils, SplitHandlesEmptySourceString)
{
	EXPECT_THAT(split("", '_'), IsEmpty());
}

TEST(StringUtils, SplitHandlesEmptyDelimiter)
{
	std::string source = "Test_with_empty_delimiter";
	EXPECT_THAT(split(source, '\0'), ElementsAre(source));
}