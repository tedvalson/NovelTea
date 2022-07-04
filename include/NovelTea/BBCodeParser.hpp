#ifndef NOVELTEA_BBCODEPARSER_HPP
#define NOVELTEA_BBCODEPARSER_HPP

#include <NovelTea/StringUtils.hpp>
#include <NovelTea/json.hpp>
#include <NovelTea/TextTypes.hpp>
#include <memory>
#include <sstream>

namespace NovelTea
{

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

private:
	static std::vector<std::pair<TextStyle,bool>> getStylesDiff(const std::vector<TextStyle> &prevStyles, const std::vector<TextStyle> &currStyles);

	template <typename citerator>
	static citerator parseTag(citerator start, citerator end, std::string &tag, bool &closing) {
		if (start == end)
			return start;

		closing = false;
		auto it = start;
		if (*++it == '/') {
			closing = true;
			++it;
		}

		std::stringstream str;
		for (; it != end; ++it) {
			auto c = *it;
			if (c == ']') {
				tag = str.str();
				return it;
			}
			else if (IsAlNum(c) || IsSpace(c) || c == '=' || c == '-' || c == '.')
				str << c;
			else
				return start;
		}
		return start;
	}
};

} // namespace NovelTea

#endif // NOVELTEA_BBCODEPARSER_HPP
