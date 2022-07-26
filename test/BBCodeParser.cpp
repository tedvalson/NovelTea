#include <gtest/gtest.h>
#include <NovelTea/BBCodeParser.hpp>
#include <iostream>

using namespace NovelTea;

bool testConversion(const std::string &bbstring, const std::string &expected)
{
	auto segments = BBCodeParser::makeSegments(bbstring);
	return (expected == BBCodeParser::makeString(segments));
}

void testConversion(const std::string &bbstring)
{
	testConversion(bbstring, bbstring);
}

TEST(BBCodeParser, Conversion) {
	EXPECT_TRUE(testConversion("[i]te[i]st", "[i]test"));
}
