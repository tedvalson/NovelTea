#include <gtest/gtest.h>
#include <NovelTea/BBCodeParser.hpp>
#include <iostream>

using namespace NovelTea;

bool testConversion(const std::string &bbstring, const std::string &expected, bool closeTags = false)
{
	auto segments = BBCodeParser::makeSegments(bbstring);
	auto str = BBCodeParser::makeString(segments, true, closeTags);
	return (expected == str);
}

bool testConversion(const std::string &bbstring)
{
	return testConversion(bbstring, bbstring);
}

TEST(BBCodeParser, Conversion) {
	EXPECT_TRUE(testConversion("[i]te[/i]st"));
	EXPECT_TRUE(testConversion("te[/i]st", "test"));
	EXPECT_TRUE(testConversion("[i]te[i]st", "[i]test"));
	EXPECT_TRUE(testConversion("[i]test", "[i]test[/i]", true));
	EXPECT_TRUE(testConversion("[i]te[i]st[/i]1", "[i]test1"));
	EXPECT_TRUE(testConversion("[i]te[p]st"));
	EXPECT_TRUE(testConversion("[i]te[p=1]st"));
}

TEST(BBCodeParser, AnimConversion) {
	EXPECT_TRUE(testConversion("[a1 e=p cs=0 t=1]test", "[a1 cs=0 e=p t=1]test[/a1]", true));
}
