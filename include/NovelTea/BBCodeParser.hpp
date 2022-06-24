#ifndef NOVELTEA_BBCODEPARSER_HPP
#define NOVELTEA_BBCODEPARSER_HPP

#include <NovelTea/StringUtils.hpp>
#include <NovelTea/json.hpp>
#include <NovelTea/TextTypes.hpp>
#include <memory>
#include <sstream>

namespace NovelTea
{
// First letters need to be unique, for now
namespace TextAnimation {
	const std::string Effect    = "effect";
	const std::string Time      = "time";
	const std::string Delay     = "delay";
	const std::string Speed     = "speed";
	const std::string CanSkip   = "cs";
	const std::string Wait      = "wait";
}

struct TextStyle {
	TextStyle()
	: type(TextStyleType::None)
	{}

	TextStyle(const std::string &tag) {
		if (!tag.empty()) {
			auto s = split(tag, " ");
			tagName = s[0];
			if (!tagName.empty()) {
				auto c = tagName[0];
				if (c == 'a' || c == 'A') {
					type = TextStyleType::Animation;
					parseKeyValPairs(tag);
					// Recreate param map, normalizing the values
					auto paramsCopy = params;
					params.clear();
					for (auto &param : paramsCopy) {
						auto c = param.first[0];
						if (c == 'e') {
							params["effect"] = param.second;
						}
						else if (c == TextAnimation::Time[0])
							params[TextAnimation::Time] = param.second;
						else if (c == TextAnimation::Delay[0])
							params[TextAnimation::Delay] = param.second;
						else if (c == TextAnimation::Speed[0])
							params[TextAnimation::Speed] = param.second;
						else if (c == TextAnimation::CanSkip[0])
							params[TextAnimation::CanSkip] = param.second;
						else if (c == TextAnimation::Wait[0])
							params[TextAnimation::Wait] = param.second;
					}
				}
				else if (c == 'b' || c == 'B') {
					type = TextStyleType::Bold;
				}
				else if (c == 'd' || c == 'D') {
					type = TextStyleType::Diff;
				}
				else if (c == 'i' || c == 'I') {
					type = TextStyleType::Italic;
				}
				else if (c == 'f' || c == 'F') {
					type = TextStyleType::Font;
					parseSingleArg(tag, "id");
				}
				else if (c == 'o' || c == 'O') {
					type = TextStyleType::Object;
					parseSingleArg(tag, "id");
				}
				else if (c == 's' || c == 'S') {
					type = TextStyleType::Size;
					parseSingleArg(tag, "size");
				}
				else
					throw std::exception();
				return;
			}
		}
		throw std::exception();
	}

	// Format: "tag=val"
	void parseSingleArg(const std::string &tag, const std::string &paramKey) {
		auto s = split(tag, "=");
		tagName = s[0];
		if (s.size() > 1)
			params[paramKey] = s[1];
	}

	// Format: "tag key=val key=val"
	void parseKeyValPairs(const std::string &tag) {
		auto s = split(tag, " ");
		for (int i = 1; i < s.size(); ++i) {
			auto kv = split(s[i], "=");
			if (kv.size() != 2)
				throw std::exception();
			auto key = kv[0];
			auto val = kv[1];
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			std::transform(val.begin(), val.end(), val.begin(), ::tolower);
			params[key] = val;
		}
	}

	bool operator==(const TextStyle &style) const
	{
		return ((type == style.type) &&
			(tagName == style.tagName) &&
			(params == style.params));
	}

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
			else if (IsAlNum(c) || c == '=' || IsSpace(c))
				str << c;
			else
				return start;
		}
		return start;
	}
};

} // namespace NovelTea

#endif // NOVELTEA_BBCODEPARSER_HPP
