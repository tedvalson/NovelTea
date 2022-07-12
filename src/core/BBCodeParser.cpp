#include <NovelTea/BBCodeParser.hpp>

namespace NovelTea
{

// First letters need to be unique, for now
namespace TextAnimation {
	const std::string Effect    = "effect";
	const std::string Function  = "func";
	const std::string Time      = "time";
	const std::string Delay     = "delay";
	const std::string Speed     = "speed";
	const std::string CanSkip   = "cs";
	const std::string Value     = "value";
	const std::string Wait      = "wait";
}

// Lowercase mapping of text effects (e.g. [anim e=FADE] will match "fade")
std::map<std::string, TextEffect> textEffectMap = {
	{"0",          TextEffect::None},
	{"n",          TextEffect::None},
	{"none",       TextEffect::None},
	{"f",          TextEffect::Fade},
	{"fade",       TextEffect::Fade},
	{"fa",         TextEffect::FadeAcross},
	{"fadeacross", TextEffect::FadeAcross},
};

TweenEngine::TweenEquation* getTweenEquation(const std::string &funcName)
{
	static std::map<std::string, TweenEngine::TweenEquation&> equationMap = {
		{"quad-in",      TweenEngine::TweenEquations::easeInQuad},
		{"quad-out",     TweenEngine::TweenEquations::easeOutQuad},
		{"quad",         TweenEngine::TweenEquations::easeInOutQuad},
		{"linear",       TweenEngine::TweenEquations::easeInOutLinear},
		{"back-in",      TweenEngine::TweenEquations::easeInBack},
		{"back-out",     TweenEngine::TweenEquations::easeOutBack},
		{"back",         TweenEngine::TweenEquations::easeInOutBack},
		{"bounce-in",    TweenEngine::TweenEquations::easeInBounce},
		{"bounce-out",   TweenEngine::TweenEquations::easeOutBounce},
		{"bounce",       TweenEngine::TweenEquations::easeInOutBounce},
		{"circ-in",      TweenEngine::TweenEquations::easeInCirc},
		{"circ-out",     TweenEngine::TweenEquations::easeOutCirc},
		{"circ",         TweenEngine::TweenEquations::easeInOutCirc},
		{"cubic-in",     TweenEngine::TweenEquations::easeInCubic},
		{"cubic-out",    TweenEngine::TweenEquations::easeOutCubic},
		{"cubic",        TweenEngine::TweenEquations::easeInOutCubic},
		{"elastic-in",   TweenEngine::TweenEquations::easeInElastic},
		{"elastic-out",  TweenEngine::TweenEquations::easeOutElastic},
		{"elastic",      TweenEngine::TweenEquations::easeInOutElastic},
		{"expo-in",      TweenEngine::TweenEquations::easeInExpo},
		{"expo-out",     TweenEngine::TweenEquations::easeOutExpo},
		{"expo",         TweenEngine::TweenEquations::easeInOutExpo},
		{"quart-in",     TweenEngine::TweenEquations::easeInQuart},
		{"quart-out",    TweenEngine::TweenEquations::easeOutQuart},
		{"quart",        TweenEngine::TweenEquations::easeInOutQuart},
		{"quint-in",     TweenEngine::TweenEquations::easeInQuint},
		{"quint-out",    TweenEngine::TweenEquations::easeOutQuint},
		{"quint",        TweenEngine::TweenEquations::easeInOutQuint},
		{"sine-in",      TweenEngine::TweenEquations::easeInSine},
		{"sine-out",     TweenEngine::TweenEquations::easeOutSine},
		{"sine",         TweenEngine::TweenEquations::easeInOutSine},
	};

	auto it = equationMap.find(funcName);
	if (it != equationMap.end())
		return &it->second;
	return nullptr;
}

std::string BBCodeParser::makeString(const std::vector<std::shared_ptr<StyledSegment>> &segments, bool shortTags)
{
	std::string result;
	std::vector<TextStyle> prevStyles;
	for (auto &s : segments) {
		auto diff = getStylesDiff(prevStyles, s->styles);
		if (s->startOnNewLine)
			result += "\n";
		if (diff.empty())
			result += "[p]";
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

std::string replaceObjectShorthand(const std::string &text)
{
	std::string result;
	size_t searchPos = 0,
		   processedPos = 0,
		   startPos;

	while ((startPos = text.find("[[", searchPos)) != text.npos)
	{
		auto endPos = text.find("]]", startPos);
		auto midPos = text.find("|", startPos);
		if (endPos == text.npos)
			break;

		// if there is no mid char "|" in between braces, skip it
		if (midPos == text.npos || endPos < midPos)
		{
			searchPos = endPos + 2;
			continue;
		}

		auto idName = text.substr(midPos + 1, endPos - midPos - 1);
		auto str = text.substr(startPos + 2, midPos - startPos - 2);
		if (startPos != processedPos)
			result += text.substr(processedPos, startPos - processedPos);
		result += "[o=" + idName + "]" + str + "[/o]";
		processedPos = searchPos = endPos + 2;
	}

	if (processedPos < text.size())
		result += text.substr(processedPos);

	return result;
}

std::vector<std::shared_ptr<StyledSegment>> BBCodeParser::makeSegments(const std::string &bbstring, const TextProperties &textDefault, const AnimationProperties &animDefault)
{
	std::vector<std::shared_ptr<StyledSegment>> result;
	std::vector<TextStyle> styleStack;
	std::stringstream str;
	bool newGroup = true;
	bool newLine = false;

	auto text = replaceObjectShorthand(bbstring);

	const auto pushSeg = [&](bool forcePush = false) {
		auto text = str.str();
		if (text.empty() && !forcePush)
			return;
		result.emplace_back(new StyledSegment(text, styleStack, textDefault, animDefault, newGroup, newLine));
		newGroup = false;
		newLine = false;
		str.str("");

		// Remove styles that should only persist for one grouping
		styleStack.erase(std::remove_if(styleStack.begin(), styleStack.end(), [](const TextStyle& style){
			return style.type == TextStyleType::XOffset || style.type == TextStyleType::YOffset;
		}), styleStack.end());
	};

	for (auto it = text.cbegin(); it != text.cend(); ++it) {
		auto c = *it;
		if (c == '[') {
			// First check for breaker tag [p]
			auto d = it;
			if (*++d == 'p' && *++d == ']') {
				it = d;
				pushSeg(true);
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
					if (style.type == TextStyleType::XOffset || style.type == TextStyleType::YOffset)
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

	// Force it to push if the previous segment does not wait for click.
	// In the case that "waitForClick" changed, this will ensure it.
	if (str.str().empty() && !result.empty()) {
		auto& lastSegment = *result.rbegin();
		if (newGroup && !lastSegment->anim.waitForClick)
			pushSeg(true);
	}
	// Push remaining content.
	pushSeg();

	return result;
}

std::string BBCodeParser::stripTags(const std::string &bbstring)
{
	std::string result;
	auto segments = makeSegments(bbstring);
	for (auto& segment : segments)
		result += segment->text;
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
	// Remove duplicates
	for (auto it = result.begin(); it != result.end(); ++it)
		for (auto it2 = it + 1; it2 != result.end(); ++it2)
			if (*it == *it2) {
				result.erase(it2);
				break;
			}
	return result;
}

cStrIter BBCodeParser::parseTag(cStrIter start, cStrIter end, std::string &tag, bool &closing)
{
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

sf::Color parseColor(const std::string &colorStr)
{
	return sf::Color::Black;
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
		switch (s.type)
		{
		case TextStyleType::None:
			break;
		case TextStyleType::Bold:
			style.fontStyle |= sf::Text::Bold;
			break;
		case TextStyleType::BorderColor:
			style.outlineColor = parseColor(s.params["color"]);
			break;
		case TextStyleType::BorderSize:
			style.outlineThickness = std::max(std::atof(s.params["size"].c_str()), 0.0);
			break;
		case TextStyleType::Color:
			style.color = parseColor(s.params["color"]);
			break;
		case TextStyleType::Diff:
			style.color = sf::Color(150, 0, 0);
			break;
		case TextStyleType::Font:
			style.fontAlias = s.params["id"];
			break;
		case TextStyleType::Italic:
			style.fontStyle |= sf::Text::Italic;
			break;
		case TextStyleType::Object:
			style.objectId = s.params["id"];
			break;
		case TextStyleType::Size:
			style.fontSize = std::atol(s.params["size"].c_str());
			break;
		case TextStyleType::XOffset:
			style.xOffset = std::atol(s.params["x"].c_str());
			break;
		case TextStyleType::YOffset:
			style.yOffset = std::atol(s.params["y"].c_str());
			break;
		case TextStyleType::Animation:
			for (auto &param : s.params) {
				auto& key = param.first;
				auto  val = param.second;
				if (key == TextAnimation::Effect) {
					auto it = textEffectMap.find(val);
					if (it != textEffectMap.end())
						anim.type = it->second;
				}
				else if (key == TextAnimation::Function) {
					auto equation = getTweenEquation(val);
					if (equation)
						anim.equation = equation;
				}
				else if (key == TextAnimation::Delay)
					anim.delay = std::max(std::atof(val.c_str()), 0.0) * 1000;
				else if (key == TextAnimation::Time)
					anim.duration = std::max(std::atof(val.c_str()), 0.0) * 1000;
				else if (key == TextAnimation::Speed)
					anim.speed = std::max(std::atof(val.c_str()), 0.01);
				else if (key == TextAnimation::CanSkip)
					anim.skippable = (val == "1");
				else if (key == TextAnimation::Value)
					anim.value = val;
				else if (key == TextAnimation::Wait)
					anim.waitForClick = (val == "1");
			}
			break;
		}
	}
}

TextStyle::TextStyle()
: type(TextStyleType::None)
{
}

TextStyle::TextStyle(const std::string &tag)
{
	if (!tag.empty()) {
		auto s = split(tag, " ");
		tagName = s[0];
		if (!tagName.empty()) {
			auto tagLower = tagName;
			std::transform(tagLower.begin(), tagLower.end(), tagLower.begin(), ::tolower);
			auto c = tagLower[0];
			if (c == 'a') {
				type = TextStyleType::Animation;
				parseKeyValPairs(tag);
				// Recreate param map, normalizing the values
				auto paramsCopy = params;
				params.clear();
				for (auto &param : paramsCopy) {
					auto c = param.first[0];
					if (c == TextAnimation::Effect[0])
						params[TextAnimation::Effect] = param.second;
					else if (c == TextAnimation::Function[0])
						params[TextAnimation::Function] = param.second;
					else if (c == TextAnimation::Time[0])
						params[TextAnimation::Time] = param.second;
					else if (c == TextAnimation::Delay[0])
						params[TextAnimation::Delay] = param.second;
					else if (c == TextAnimation::Speed[0])
						params[TextAnimation::Speed] = param.second;
					else if (c == TextAnimation::CanSkip[0])
						params[TextAnimation::CanSkip] = param.second;
					else if (c == TextAnimation::Value[0])
						params[TextAnimation::Value] = param.second;
					else if (c == TextAnimation::Wait[0])
						params[TextAnimation::Wait] = param.second;
				}
			}
			else if (c == 'b') {
				type = TextStyleType::Bold;
				if (tagLower.size() > 1) {
					auto c2 = tagLower[1];
					if (c2 == 'c' || tagLower == "border-color") {
						type = TextStyleType::BorderColor;
						parseSingleArg(tag, "color");
					} else if (c2 == 's' || tagLower == "border-size") {
						type = TextStyleType::BorderSize;
						parseSingleArg(tag, "size");
					} else
						throw std::exception();
				}
			}
			else if (c == 'c') {
				type = TextStyleType::Color;
				parseSingleArg(tag, "color");
			}
			else if (c == 'd') {
				type = TextStyleType::Diff;
			}
			else if (c == 'i') {
				type = TextStyleType::Italic;
			}
			else if (c == 'f') {
				type = TextStyleType::Font;
				parseSingleArg(tag, "id");
			}
			else if (c == 'o') {
				type = TextStyleType::Object;
				parseSingleArg(tag, "id");
			}
			else if (c == 's') {
				type = TextStyleType::Size;
				parseSingleArg(tag, "size");
			}
			else if (c == 'x') {
				type = TextStyleType::XOffset;
				parseSingleArg(tag, "x");
			}
			else if (c == 'y') {
				type = TextStyleType::YOffset;
				parseSingleArg(tag, "y");
			}
			else
				throw std::exception();
			return;
		}
	}
	throw std::exception();
}

void TextStyle::parseSingleArg(const std::string &tag, const std::string &paramKey)
{
	auto s = split(tag, "=");
	tagName = s[0];
	if (s.size() > 1)
		params[paramKey] = s[1];
}

void TextStyle::parseKeyValPairs(const std::string &tag)
{
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

bool TextStyle::operator==(const TextStyle &style) const
{
	return ((type == style.type) &&
		(tagName == style.tagName) &&
		(params == style.params));
}

} // namespace NovelTea
