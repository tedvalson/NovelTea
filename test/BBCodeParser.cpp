#include <gtest/gtest.h>
#include <NovelTea/BBCodeParser.hpp>
#include <iostream>

using namespace NovelTea;

void testConversion(const std::string &bbstring, const std::string &expected, bool closeTags = false)
{
	auto segments = BBCodeParser::makeSegments(bbstring);
	auto result = BBCodeParser::makeString(segments, true, closeTags);
	EXPECT_EQ(expected, result);
}

void testConversion(const std::string &bbstring)
{
	testConversion(bbstring, bbstring);
}

TEST(BBCodeParser, Conversion) {
	testConversion("[i]te[/i]st");
	testConversion("te[/i]st", "test");
	testConversion("[i]te[i]st", "[i]test");
	testConversion("[i]test", "[i]test[/i]", true);
	testConversion("[i]te[i]st[/i]1", "[i]test1");
	testConversion("[i]te[p]st");
	testConversion("[i]te[p=1]st");
	testConversion("te[!i]st[!/i]");
}

TEST(BBCodeParser, AnimConversion) {
	testConversion("[a1 e=p cs=0 t=1]test", "[a1 cs=0 e=p t=1]test[/a1]", true);
}
