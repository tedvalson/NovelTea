#include <NovelTea/BBCodeParser.hpp>

namespace NovelTea
{

std::map<std::string, TextEffect> textEffectMap = {
	{"0",          TextEffect::None},
	{"none",       TextEffect::None},
	{"f",          TextEffect::Fade},
	{"fade",       TextEffect::Fade},
	{"fa",         TextEffect::FadeAcross},
	{"fadeacross", TextEffect::FadeAcross},
};

std::string BBCodeParser::makeString(const std::vector<std::shared_ptr<StyledSegment>> &segments, bool shortTags)
{
	std::string result;
	std::vector<TextStyle> prevStyles;
	for (auto &s : segments) {
		auto diff = getStylesDiff(prevStyles, s->styles);
		for (auto &p : diff) {
			auto &style = p.first;
			if (p.second) {
				result += "["+style.tagName;
				if (!style.params.empty()) {
					if (style.tagName[0] == 'o') {
						result += "=" + style.params["id"];
					}
					else
						for (auto &param : style.params)
							result += " " + param.first + "=" + param.second;
				}
				result += "]";
			} else {
				result += "[/"+style.tagName+"]";
			}
		}
		result += s->text;
		prevStyles = s->styles;
	}
	return result;
}

std::vector<std::shared_ptr<StyledSegment>> BBCodeParser::makeSegments(const std::string &text, const TextProperties &textDefault, const AnimationProperties &animDefault)
{
	std::vector<std::shared_ptr<StyledSegment>> result;
	std::vector<TextStyle> styleStack;
	std::stringstream str;
	bool newGroup = true;
	bool newLine = false;

	const auto pushSeg = [&]() {
		auto text = str.str();
		if (text.empty())
			return;
		result.emplace_back(new StyledSegment(text, styleStack, textDefault, animDefault, newGroup, newLine));
		newGroup = false;
		newLine = false;
		str.str("");
	};

	for (auto it = text.cbegin(); it != text.cend(); ++it) {
		auto c = *it;
		if (c == '[') {
			// First check for breaker tag [p]
			auto d = it;
			if (*++d == 'p' && *++d == ']') {
				it = d;
				pushSeg();
				newGroup = true;
				continue;
			}

			std::string tag;
			bool closing;
			auto r = parseTag(it, text.cend(), tag, closing);
			if (r == it) {
				str << c;
			}
			else
				try {
					TextStyle style(tag);
					auto itStyle = styleStack.rbegin();
					if (closing) {
						itStyle = std::find_if(itStyle, styleStack.rend(), [style](TextStyle &s){
							return s.tagName == style.tagName;
						});
						if (itStyle == styleStack.rend())
							throw std::exception();
					}

					pushSeg();

					// All animation tags create new groups
					if (style.type == TextStyleType::Animation)
						newGroup = true;

					if (closing)
						styleStack.erase(std::next(itStyle).base());
					else
						styleStack.push_back(style);

					it = r;
				} catch (std::exception &e) {
					str << c;
				}
		}
		else if (c == '\n') {
			pushSeg();
			newLine = true;
		}
		else
			str << c;
	}

	// Push remaining content
	pushSeg();

	return result;
}

std::vector<std::pair<TextStyle, bool> > BBCodeParser::getStylesDiff(const std::vector<TextStyle> &prevStyles, const std::vector<TextStyle> &currStyles)
{
	std::vector<std::pair<TextStyle,bool>> result;
	for (auto &prevStyle : prevStyles) {
		auto p = std::find(currStyles.begin(), currStyles.end(), prevStyle);
		if (p == currStyles.end())
			result.emplace_back(prevStyle, false);
	}
	for (auto &currStyle : currStyles) {
		auto p = std::find(prevStyles.begin(), prevStyles.end(), currStyle);
		if (p == prevStyles.end())
			result.emplace_back(currStyle, true);
	}
	return result;
}

StyledSegment::StyledSegment(std::string text, std::vector<TextStyle> styles, const TextProperties &textDefault, const AnimationProperties &animDefault, bool newGroup, bool startOnNewLine)
	: text(text)
	, styles(styles)
	, style(textDefault)
	, anim(animDefault)
	, newGroup(newGroup)
	, startOnNewLine(startOnNewLine)
{
	for (auto &s : styles) {
		if (s.type == TextStyleType::Bold)
			style.fontStyle |= sf::Text::Bold;
		else if (s.type == TextStyleType::Italic)
			style.fontStyle |= sf::Text::Italic;
		else if (s.type == TextStyleType::Diff)
			style.color = sf::Color(150, 0, 0);
		else if (s.type == TextStyleType::Object)
			style.objectId = s.params["id"];
		else if (s.type == TextStyleType::Font)
			style.fontAlias = s.params["id"];
		else if (s.type == TextStyleType::Size)
			style.fontSize = std::atol(s.params["size"].c_str());
		else if (s.type == TextStyleType::Animation) {
			for (auto &param : s.params) {
				auto& key = param.first;
				auto  val = param.second;
				if (key == TextAnimation::Effect) {
					auto it = textEffectMap.find(val);
					if (it != textEffectMap.end())
						anim.type = it->second;
				}
				else if (key == TextAnimation::Delay)
					anim.delay = std::atol(val.c_str());
				else if (key == TextAnimation::Time)
					anim.duration = std::atol(val.c_str());
				else if (key == TextAnimation::Speed)
					anim.speed = std::max(std::atof(val.c_str()), 0.01);
				else if (key == TextAnimation::CanSkip)
					anim.skippable = (val == "1");
				else if (key == TextAnimation::Wait)
					anim.waitForClick = (val == "1");
			}
		}
	}
}

} // namespace NovelTea
