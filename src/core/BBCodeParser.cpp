#include <NovelTea/BBCodeParser.hpp>
#include <iostream>
#include <algorithm>

namespace NovelTea
{

// First letters need to be unique, for now
namespace TextAnimation {
	const std::string Effect    = "effect";
	const std::string Function  = "func";
	const std::string Time      = "time";
	const std::string Delay     = "delay";
	const std::string Loop      = "loop";
	const std::string Speed     = "speed";
	const std::string CanSkip   = "cs";
	const std::string Value     = "value";
	const std::string Wait      = "wait";
}

// Lowercase mapping of text effects (e.g. [anim e=FADE] will match "fade")
std::map<std::string, TextEffect> textEffectMap = {
	{"0",          TextEffect::None},
	{"no",         TextEffect::None},
	{"none",       TextEffect::None},
	{"f",          TextEffect::Fade},
	{"fade",       TextEffect::Fade},
	{"fa",         TextEffect::FadeAcross},
	{"fadeacross", TextEffect::FadeAcross},
	{"n",          TextEffect::Nod},
	{"nod",        TextEffect::Nod},
	{"s",          TextEffect::Shake},
	{"shake",      TextEffect::Shake},
	{"p",          TextEffect::Pop},
	{"pop",        TextEffect::Pop},
	{"t",          TextEffect::Tremble},
	{"tremble",    TextEffect::Tremble},
	{"test",       TextEffect::Test},
};

std::map<std::string, Color> colorMap = {
	{"black",    {0, 0, 0}},
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

std::string BBCodeParser::makeString(const std::vector<std::shared_ptr<StyledSegment>> &segments, bool shortTags, bool closeTags, bool escapeTags)
{
	std::string result;
	std::string tagOpener = escapeTags ? "[!" : "[";
	std::string tagCloser = escapeTags ? "[!/" : "[/";
	std::vector<TextStyle> prevStyles;
	for (auto &s : segments) {
		auto diff = getStylesDiff(prevStyles, s->styles);
		if (s->startOnNewLine)
			result += "\n";
		for (auto &p : diff) {
			auto &style = p.first;
			// Handle PBreak specially, ignoring "opening" tag
			if (style.tagName == "p") {
				if (!p.second) {
					result += tagOpener + "p";
					if (!style.params.empty())
						result += "=" + style.params["delay"];
					result += "]";
				}
				continue;
			}
			// If style is new (ie. opening tag)
			if (p.second) {
				result += tagOpener + style.tagName;
				if (!style.params.empty()) {
					if (style.tagName[0] == 'o') {
						result += "=" + style.params["id"];
					}
					else
						for (auto &param : style.params) {
							if (shortTags) {
								auto name = param.first;
								if (name != TextAnimation::CanSkip)
									name = name.substr(0,1);
								result += " " + name + "=" + param.second;
							} else
								result += " " + param.first + "=" + param.second;
						}
				}
				result += "]";
			} else {
				result += tagCloser + style.tagName+"]";
			}
		}

		// Parse again to find tags, meaning they need to be escaped.
		// Space is appended as a hack to handle escaped closing tags, eg. "te[!i]st[!/i]"
		auto segs = makeSegments(s->text + " ");
		if (segs.size() > 1) {
			auto s = makeString(segs, shortTags, false, true);
			s.erase(s.size()-1);
			result += s;
		} else
			result += s->text;

		prevStyles = s->styles;
	}
	if (closeTags) {
		std::vector<std::string> closedTags;
		for (auto& style : prevStyles) {
			auto& tag = style.tagName;
			if (std::find(closedTags.begin(), closedTags.end(), tag) == closedTags.end()) {
				closedTags.push_back(tag);
				result += tagCloser + tag + "]";
			}
		}
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
		if (text.empty() && !newLine && !forcePush)
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
			// First check for "!" tag cancel symbol
			if (*(it + 1) == '!') {
				str << c;
				++it;
				continue;
			}

			std::string tag;
			auto r = parseTag(it, text.cend(), tag);
			if (r == it) {
				str << c;
			}
			else
				try {
					bool closing;
					TextStyle style(tag, closing);

					// First check for breaker tag [p]
					if (style.type == TextStyleType::PBreak) {
						if (closing)
							throw std::exception();
						// [p] is self-closing tag, so push then pop
						styleStack.push_back(style);
						pushSeg(true);
						styleStack.pop_back();
						newGroup = true;
						it = r;
						continue;
					}

					auto itStyle = styleStack.rbegin();
					if (closing) {
						itStyle = std::find_if(itStyle, styleStack.rend(), [style](TextStyle &s){
							return s.tagName == style.tagName;
						});
						// Ignore (and remove) closing tags that were never opened
						if (itStyle == styleStack.rend()) {
							it = r;
							continue;
						}
					}

					pushSeg();

					// All animation tags create new groups
					if (style.type == TextStyleType::Animation)
						newGroup = true;
					if (style.type == TextStyleType::XOffset || style.type == TextStyleType::YOffset)
						newGroup = true;
					if (style.type == TextStyleType::Shader)
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
	// Push remaining content. And if text was empty, force it to make at least one segment.
	pushSeg(text.empty());

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

void BBCodeParser::debugPrint(const std::vector<std::shared_ptr<StyledSegment> > &segments)
{
	for (auto &s : segments) {
		std::cout << "[";
		for (auto &style : s->styles)
			std::cout << " +" << style.tagName;
		std::cout << "] " << (s->newGroup ? "new" : "old") << (s->startOnNewLine ? " \\n" : "");
		std::cout << " \"" << s->text << "\" " << s->style.fontSize;
		std::cout << " " << (s->anim.waitForClick ? "wait" : "") <<  std::endl;
	}
	std::cout << std::endl;
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

cStrIter BBCodeParser::parseTag(cStrIter start, cStrIter end, std::string &tag)
{
	if (start == end)
		return start;

	// See MainWindow::validateEntityName
	// For Object tag, entity naming needs to be restricted to these chars.
	static const char allowedChars[] = " =_,.-#";

	std::stringstream str;
	for (auto it = start + 1; it != end; ++it) {
		auto c = *it;
		if (IsAlNum(c) || std::strchr(allowedChars, c) != nullptr)
			str << c;
		else if (c == ']') {
			tag = str.str();
			return it;
		}
		else if (c == '/') {
			if (it == start + 1)
				str << c;
			else
				return start;
		}
		else
			return start;
	}
	return start;
}

Color parseColor(std::string colorStr)
{
	auto result = Color{0, 0, 0};
	std::transform(colorStr.begin(), colorStr.end(), colorStr.begin(), ::tolower);

	// Check for predefined color matches if no "#" prefix
	if (colorStr[0] != '#') {
		auto it = colorMap.find(colorStr);
		if (it != colorMap.end())
			return it->second;
		return result;
	}

	// Erase "#"
	colorStr.erase(0, 1);

	while (colorStr.size() < 3)
		colorStr.insert(0, 1, '0');

	// Convert short format to long (eg. BED -> BBEEDD)
	if (colorStr.size() == 3) {
		colorStr += colorStr[2];
		colorStr.insert(1, 1, colorStr[1]);
		colorStr.insert(0, 1, colorStr[0]);
	}

	while (colorStr.size() < 6)
		colorStr += '0';

	if (colorStr.size() != 6)
		return result;

	int v = std::strtol(colorStr.c_str(), 0, 16);
	result.r = (v >> 16) & 0xFF;
	result.g = (v >> 8) & 0xFF;
	result.b = v & 0xFF;

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
		switch (s.type)
		{
		case TextStyleType::None:
			break;
		case TextStyleType::Bold:
			style.fontStyle |= FontBold;
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
			style.color = Color{150, 0, 0};
			break;
		case TextStyleType::Font:
			style.fontAlias = s.params["id"];
			break;
		case TextStyleType::Italic:
			style.fontStyle |= FontItalic;
			break;
		case TextStyleType::Object:
			style.objectId = s.params["id"];
			break;
		case TextStyleType::PBreak:
			if (!s.params.empty()) {
				auto delay = std::max(std::atof(s.params["delay"].c_str()), 0.0) * 1000;
				// Negative delays considered for FadeAcross effect
				anim.delay += (anim.delay >= 0) ? delay : -delay;
				anim.waitForClick = false;
			}
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
		case TextStyleType::Shader: {
			auto fragShaderId = s.params["f"];
			auto vertexShaderId = s.params["v"];
			if (!fragShaderId.empty())
				style.fragShaderId = fragShaderId;
			if (!vertexShaderId.empty())
				style.vertexShaderId = vertexShaderId;
			for (auto& param : s.params) {
				auto& key = param.first;
				if (key == "f" || key == "v")
					continue;
				style.shaderUniforms[key] = std::atof(param.second.c_str());
			}
			break;
		}
		case TextStyleType::Animation:
			for (auto &param : s.params) {
				auto& key = param.first;
				auto  val = param.second;
				if (key == TextAnimation::Effect) {
					auto it = textEffectMap.find(val);
					if (it != textEffectMap.end())
						anim.type = it->second;

					// Change FadeAcross effects to special default delay/duration
					int defaultDelay = (anim.type == TextEffect::FadeAcross) ? -1 : 0;
					int defaultDuration = defaultDelay;
					if (s.params.find(TextAnimation::Delay) == s.params.end())
						anim.delay = defaultDelay;
					if (s.params.find(TextAnimation::Time) == s.params.end())
						anim.duration = defaultDuration;
				}
				else if (key == TextAnimation::Function) {
					auto equation = getTweenEquation(val);
					if (equation)
						anim.equation = equation;
				}
				else if (key == TextAnimation::Loop) {
					auto v = split(val, ",");
					if (v.size() > 1) {
						anim.loopCount = std::atol(v[0].c_str());
						anim.loopDelay = std::atof(v[1].c_str()) * 1000;
						if (v.size() > 2)
							anim.loopYoyo = v[2] != "0";
					}
					else
						anim.loopCount = std::atol(val.c_str());
				}
				else if (key == TextAnimation::Delay)
					anim.delay = std::atof(val.c_str()) * 1000;
				else if (key == TextAnimation::Time)
					anim.duration = std::atof(val.c_str()) * 1000;
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

TextStyle::TextStyle(const std::string &tagFull, bool &closing)
{
	if (!tagFull.empty()) {
		auto tag = tagFull;
		closing = (tag[0] == '/');
		if (closing)
			tag.erase(0, 1);
		auto s = split(tag, " ");
		tagName = s[0];
		if (!tagName.empty()) {
			auto tagLower = tag;
			std::transform(tagLower.begin(), tagLower.end(), tagLower.begin(), ::tolower);
			auto c = tagLower[0];
			if (c == 'a') {
				type = TextStyleType::Animation;
				parseKeyValPairs(tag, true);
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
					else if (c == TextAnimation::Loop[0])
						params[TextAnimation::Loop] = param.second;
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
					auto s = split(tagLower, "=");
					if (closing)
						s.push_back("");
					tagName = s[0];
					if (s.size() == 2) {
						auto c2 = tagLower[1];
						if (c2 == 'c' || s[0] == "border-color") {
							type = TextStyleType::BorderColor;
							params["color"] = s[1];
						} else if (c2 == 's' || s[0] == "border-size") {
							type = TextStyleType::BorderSize;
							params["size"] = s[1];
						}
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
			else if (c == 'p') {
				type = TextStyleType::PBreak;
				parseSingleArg(tag, "delay");
			}
			else if (c == 's') {
				if (tagLower.size() > 1 && tagLower[1] == 'h') {
					type = TextStyleType::Shader;
					parseKeyValPairs(tag, false);
					if (params.empty() && !closing)
						throw std::exception();
				} else {
					type = TextStyleType::Size;
					parseSingleArg(tag, "size");
				}
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

void TextStyle::parseKeyValPairs(const std::string &tag, bool makeLowerCase)
{
	auto s = split(tag, " ");
	for (int i = 1; i < s.size(); ++i) {
		auto kv = split(s[i], "=");
		if (kv.size() != 2)
			throw std::exception();
		auto key = kv[0];
		auto val = kv[1];
		if (makeLowerCase) {
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			std::transform(val.begin(), val.end(), val.begin(), ::tolower);
		}
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
