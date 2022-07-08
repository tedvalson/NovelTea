#ifndef NOVELTEA_BBCODEPARSER_HPP
#define NOVELTEA_BBCODEPARSER_HPP

#include <NovelTea/StringUtils.hpp>
#include <NovelTea/TextTypes.hpp>
#include <memory>
#include <sstream>

namespace NovelTea
{

using cStrIter = std::string::const_iterator;

struct TextStyle {
	TextStyle();
	TextStyle(const std::string &tag);

	// Format: "tag=val"
	void parseSingleArg(const std::string &tag, const std::string &paramKey);

	// Format: "tag key=val key=val"
	void parseKeyValPairs(const std::string &tag);

	bool operator==(const TextStyle &style) const;

	TextStyleType type;
	std::string tagName;
	std::map<std::string, std::string> params;
};

struct StyledSegment {
	StyledSegment(std::string text,	std::vector<TextStyle> styles, const TextProperties &textDefault, const AnimationProperties &animDefault, bool newGroup, bool startOnNewLine);
	std::string text;
	std::vector<TextStyle> styles;
	TextProperties style;
	AnimationProperties anim;
	bool newGroup;
	bool startOnNewLine;
};


class BBCodeParser
{
public:
	static std::string makeString(const std::vector<std::shared_ptr<StyledSegment>> &segments, bool shortTags = true);
	static std::vector<std::shared_ptr<StyledSegment>> makeSegments(const std::string &text, const TextProperties &textDefault = TextProperties(), const AnimationProperties &animDefault = AnimationProperties());
	static std::string stripTags(const std::string &bbstring);

private:
	static std::vector<std::pair<TextStyle,bool>> getStylesDiff(const std::vector<TextStyle> &prevStyles, const std::vector<TextStyle> &currStyles);
	static cStrIter parseTag(cStrIter start, cStrIter end, std::string &tag, bool &closing);
};

} // namespace NovelTea

#endif // NOVELTEA_BBCODEPARSER_HPP
