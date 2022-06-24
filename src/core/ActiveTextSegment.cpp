#include <NovelTea/ActiveTextSegment.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/BBCodeParser.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/StringUtils.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

ActiveTextSegment::ActiveTextSegment()
	: m_lineMaxCharSize(0)
	, m_size(1024.f, 1024.f)
	, m_needsUpdate(true)
	, m_lineSpacing(5.f)
	, m_alpha(255.f)
	, m_animAlpha(255.f)
	, m_highlightFactor(1.f)
	, m_fontSizeMultiplier(1.f)
	, m_fadeAcrossPosition(1.f)
	, m_fadeLineIndex(0)
	, m_renderTexture(nullptr)
{
	auto texture = AssetManager<sf::Texture>::get("images/fade.png");
	texture->setSmooth(true);
	m_shape.setFillColor(sf::Color::Transparent);
	m_shapeFade.setTexture(texture.get(), true);
	m_shapeFade.setSize(sf::Vector2f(300.f, 40.f));
	m_shapeFade.setOrigin(0.f, 40.f);
	m_shapeFade.setRotation(90.f);
}

ActiveTextSegment::ActiveTextSegment(const std::vector<std::shared_ptr<StyledSegment>> &segments)
	: ActiveTextSegment()
{
	setStyledSegments(segments);
}

void ActiveTextSegment::createRenderTexture()
{
	m_shape.setSize(m_size);
	m_renderTexture = std::make_shared<sf::RenderTexture>();
	m_renderTexture->create(m_size.x, 512); // TODO: Avoid fixed size
	m_sprite.setTexture(m_renderTexture->getTexture(), true);
}

json ActiveTextSegment::toJson() const
{
	json j = sj::Array();
	return j;
}

bool ActiveTextSegment::fromJson(const json &j)
{
	m_needsUpdate = true;
	return true;
}

std::string ActiveTextSegment::toPlainText(bool stripBBCodes, const std::string &newlineChar) const
{
	std::string result;
	return result;
}

std::string ActiveTextSegment::objectFromPoint(const sf::Vector2f &point) const
{
	ensureUpdate();
	for (auto &segment : m_segments)
	{
		if (!segment.objectIdName.empty())
		{
			auto bounds = getTransform().transformRect(segment.bounds);
			if (bounds.contains(point))
				return segment.objectIdName;
		}
	}
	return std::string();
}

void ActiveTextSegment::setStyledSegments(const std::vector<std::shared_ptr<StyledSegment>> &segments)
{

	m_styledSegments = segments;
	m_string = "styledSegments";
	m_needsUpdate = true;
}

void ActiveTextSegment::setText(const std::string &text)
{
	setText(text, TextProperties(), AnimationProperties());
}

void ActiveTextSegment::setText(const std::string &text, const AnimationProperties &animProps)
{
	setText(text, TextProperties(), animProps);
}

void ActiveTextSegment::setText(const std::string &text, const TextProperties &textProps)
{
	setText(text, textProps, AnimationProperties());
}

void ActiveTextSegment::setText(const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps)
{
	auto styledSegments = BBCodeParser::makeSegments(text, textProps, animProps);
	setStyledSegments(styledSegments);
	m_string = text;
}

std::string ActiveTextSegment::getText() const
{
	return m_string;
}

void ActiveTextSegment::startAnim()
{
	auto& anim = getAnimProps();
	auto delay = 0.001f * getDelayMs();
	auto duration = 0.001f * getDurationMs();
	m_tweenManager.killAll();

	setFadeAcrossPosition(1.f);
	setAnimAlpha(255.f);

	if (anim.type == TextEffect::Fade) {
		setAnimAlpha(0.f);
		TweenEngine::Tween::to(*this, ANIMALPHA, duration)
			.target(255.f)
			.start(m_tweenManager);
	}
	else if (anim.type == TextEffect::FadeAcross) {
		setFadeAcrossPosition(0.f);
		TweenEngine::Tween::to(*this, FADEACROSS, duration)
			.ease(TweenEngine::TweenEquations::easeInOutLinear)
			.target(1.f)
			.start(m_tweenManager);
	}
	m_tweenManager.update(0.f); // TODO: why is this needed?????
}

void ActiveTextSegment::setSize(const sf::Vector2f &size)
{
	m_needsUpdate = true;
	m_size = size;
	if (m_renderTexture)
		createRenderTexture();
}

sf::Vector2f ActiveTextSegment::getSize() const
{
	return m_size;
}

void ActiveTextSegment::setLastLineMaxHeight(unsigned int height)
{
	m_lineMaxCharSize = height;
	m_needsUpdate = true;
}

unsigned int ActiveTextSegment::getCurrentLineMaxHeight() const
{
	ensureUpdate();
	return m_lineMaxCharSize;
}

void ActiveTextSegment::setHighlightId(const std::string &id)
{
	for (auto &segment : m_segments)
	{
		if (id.empty() || id != segment.objectIdName)
		{
			segment.text.setOutlineThickness(0.f);
		}
		else
		{
			segment.text.setOutlineColor(sf::Color(255, 255, 0, 140));
			segment.text.setOutlineThickness(0.1f * segment.text.getCharacterSize());
		}
	}
}

void ActiveTextSegment::refresh()
{
	m_needsUpdate = true;
}

float ActiveTextSegment::getTextWidth() const
{
	auto width = 0.f;
	for (auto &segment : m_segments)
		width += segment.text.getLocalBounds().width;
	return width;
}

size_t ActiveTextSegment::getDelayMs() const
{
	auto& anim = getAnimProps();
	auto delay = anim.delay;
	if (anim.type == TextEffect::FadeAcross && delay < 0)
		delay = 1000.f * getFadeAcrossLength() / 220.f * 2;
	return delay;
}

size_t ActiveTextSegment::getDurationMs() const
{
	auto& anim = getAnimProps();
	auto duration = anim.duration;
	if (anim.type == TextEffect::FadeAcross && duration < 0)
		duration = 1000.f * getFadeAcrossLength() / 220.f * 2;
	return duration;
}

sf::FloatRect ActiveTextSegment::getLocalBounds() const
{
	ensureUpdate();
	return m_bounds;
}

sf::FloatRect ActiveTextSegment::getGlobalBounds() const
{
	ensureUpdate();
	return getTransform().transformRect(m_bounds);
}

void ActiveTextSegment::setLineSpacing(float lineSpacing)
{
	m_needsUpdate = true;
	m_lineSpacing = lineSpacing;
}

float ActiveTextSegment::getLineSpacing() const
{
	return m_lineSpacing;
}

void ActiveTextSegment::setCursorStart(const sf::Vector2f &cursorPos)
{
	m_needsUpdate = true;
	m_cursorStart = cursorPos;
}

const sf::Vector2f &ActiveTextSegment::getCursorEnd() const
{
	ensureUpdate();
	return m_cursorPos;
}

void ActiveTextSegment::setAlpha(float alpha)
{
	m_alpha = alpha;
	applyAlpha();
}

float ActiveTextSegment::getAlpha() const
{
	return m_alpha;
}

void ActiveTextSegment::setHighlightFactor(float highlightFactor)
{
	m_highlightFactor = highlightFactor;
	applyHighlightFactor();
}

float ActiveTextSegment::getHighlightFactor() const
{
	return m_highlightFactor;
}

void ActiveTextSegment::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_needsUpdate = true;
	m_fontSizeMultiplier = fontSizeMultiplier;
}

float ActiveTextSegment::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
}

void ActiveTextSegment::setFadeAcrossPosition(float position)
{
	ensureUpdate();
	m_fadeAcrossPosition = position;
	if (m_segments.empty())
		return;
	if (position > 0.9999f) {
		m_renderTexture = nullptr;
	} else {
		if (!m_renderTexture)
			createRenderTexture();

		auto fadeLength = getFadeAcrossLength();
		auto startX = m_segments[0].text.getPosition().x;
		auto pos = m_fadeAcrossPosition * fadeLength;
		auto p = 0.f;

		m_fadeLineIndex = 0;
		for (auto &line : m_linePositions)
		{
			auto w = line.x;
			if (m_fadeLineIndex == 0)
				w -= startX;
			p += w + m_shapeFade.getSize().y;
			if (p > pos) {
				m_shape.setPosition(w - p + pos, line.y);
				if (m_fadeLineIndex == 0)
					m_shape.move(startX, 0.f);
				m_shapeFade.setPosition(m_shape.getPosition());
				m_shape.move(m_shapeFade.getSize().y, 0.f);
				break;
			}
			m_fadeLineIndex++;
		}
	}
}

float ActiveTextSegment::getFadeAcrossPosition() const
{
	return m_fadeAcrossPosition;
}

float ActiveTextSegment::getFadeAcrossLength() const
{
	ensureUpdate();
	auto len = 0.f;
	if (m_segments.empty())
		return len;
	for (auto &linePos : m_linePositions)
		len += linePos.x + m_shapeFade.getSize().y;
	return len - m_segments[0].text.getPosition().x;
}

std::vector<ActiveTextSegment::Segment> &ActiveTextSegment::getSegments()
{
	ensureUpdate();
	return m_segments;
}

AnimationProperties &ActiveTextSegment::getAnimProps() const
{
	assert(!m_styledSegments.empty());
	return m_styledSegments[0]->anim;
}

bool ActiveTextSegment::update(float delta)
{
	m_tweenManager.update(delta);
	return Hideable::update(delta);
}

void ActiveTextSegment::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case HIGHLIGHTS:
			setHighlightFactor(newValues[0]);
			break;
		case FADEACROSS:
			setFadeAcrossPosition(newValues[0]);
			break;
		case ANIMALPHA:
			setAnimAlpha(newValues[0]);
			break;
		default:
			Hideable::setValues(tweenType, newValues);
	}
}

int ActiveTextSegment::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case HIGHLIGHTS:
		returnValues[0] = getHighlightFactor();
		return 1;
	case FADEACROSS:
		returnValues[0] = getFadeAcrossPosition();
		return 1;
	case ANIMALPHA:
		returnValues[0] = m_animAlpha;
		return 1;
	default:
		return Hideable::getValues(tweenType, returnValues);
	}
}

void ActiveTextSegment::setAnimAlpha(float alpha)
{
	m_animAlpha = alpha;
	applyAlpha();
}

void ActiveTextSegment::applyAlpha() const
{
	sf::Color color;
	const float *newValues = &m_alpha; // Hack for the macro below
	for (auto &segment : m_segments) {
		SET_ALPHA(segment.text.getFillColor, segment.text.setFillColor, m_animAlpha);
	}
}

void ActiveTextSegment::applyHighlightFactor() const
{
	auto alpha = m_alpha * m_animAlpha / 255.f;
	for (auto &segment : m_segments)
	{
		if (!segment.objectIdName.empty())
		{
			sf::Color color;
			if (segment.objectExists)
				color = sf::Color(0, 0, m_highlightFactor * 200, alpha);
			else
				color = sf::Color(m_highlightFactor * 155, 0, m_highlightFactor * 255, alpha);
			segment.text.setFillColor(color);
		}
	}
}

void ActiveTextSegment::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	if (m_segments.empty() || m_alpha == 0.f || m_animAlpha == 0.f)
		return;
	states.transform *= getTransform();

	if (m_renderTexture)
	{
		auto posY = m_segments[0].text.getPosition().y;
		auto lineIndex = 0;
		m_renderTexture->clear(sf::Color::Transparent);
		for (auto &segment : m_segments)
		{
			auto p = segment.text.getPosition().y;
			if (p > posY) {
				posY = p;
				lineIndex++;
			}
			if (lineIndex == m_fadeLineIndex)
				m_renderTexture->draw(segment.text, sf::BlendNone);
			else if (lineIndex > m_fadeLineIndex)
				break;
			else
				target.draw(segment.text, states);
		}
		m_renderTexture->draw(m_shape, sf::BlendMultiply);
		m_renderTexture->draw(m_shapeFade, sf::BlendMultiply);
		m_renderTexture->display();
		target.draw(m_sprite, states);
	}
	else
	{
		target.draw(m_debugBorder, states);
		for (auto &shape : m_debugSegmentShapes)
			target.draw(shape, states);

		for (auto &segment : m_segments)
			target.draw(segment.text, states);

	}
}

void ActiveTextSegment::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	auto padding = 6.f;
	float lineHeight = 4.f; // TODO: Don't use fixed value
	m_cursorPos = m_cursorStart;
	m_segments.clear();
	m_linePositions.clear();
	m_debugSegmentShapes.clear();
	m_bounds = sf::FloatRect(0.f, m_cursorStart.y, m_cursorStart.x, m_cursorStart.y + lineHeight);

	sf::RectangleShape debugShape;
	debugShape.setFillColor(sf::Color(0, 0, 0, 30));

	for (auto &styledSegment : m_styledSegments)
	{
		if (styledSegment->startOnNewLine && m_cursorPos.x > 0)
		{
			m_linePositions.push_back(m_cursorPos);
			m_cursorPos.y = m_bounds.height;
			m_bounds.height += m_lineMaxCharSize + m_lineSpacing;
			m_lineMaxCharSize = 0;
			m_cursorPos.x = 0.f;
		}

		auto& style = styledSegment->style;
		auto& objectId = style.objectId;
		auto fontAlias = style.fontAlias;
		if (fontAlias.empty())
			fontAlias = ProjData[ID::projectFontDefault].ToString();
		auto font = Proj.getFont(fontAlias);
		auto objectExists = false;
		auto color = style.color;
		if (!objectId.empty()) {
			objectExists = ActiveGame->getRoom()->containsId(objectId) ||
						  ActiveGame->getObjectList()->containsId(objectId);
		}
		color.a = m_alpha;

		auto words = split(styledSegment->text, " ");
		auto firstWord = true;
		for (auto &word : words)
		{
			TweenText text;
			text.setFont(*font);
			text.setCharacterSize(2.f * m_fontSizeMultiplier * style.fontSize);
			text.setStyle(style.fontStyle);
			text.setOutlineColor(style.outlineColor);
			text.setOutlineThickness(style.outlineThickness);

			text.setString(" ");
			auto spaceWidth = text.getLocalBounds().width;

			if (word.empty())
			{
				// Don't start line with a space
				if (m_cursorPos.x > 0)
					m_cursorPos.x += spaceWidth;
				continue;
			}
			if (firstWord)
				firstWord = false;
			else
				m_cursorPos.x += spaceWidth;

			sf::String string = word;
			text.setString(string);
			text.setFillColor(color);
			m_lineMaxCharSize = std::max(m_lineMaxCharSize, text.getCharacterSize());

			// Hack to prevent these chars from wrapping by themselves.
			const sf::String specialChars = ".,!?";
			auto isSpecialChar = (string.getSize() == 1 && specialChars.find(string) != sf::String::InvalidPos);

			// Wrap text to next line if it exceeds the width
			auto newX = m_cursorPos.x + text.getLocalBounds().width;
			if (newX > m_size.x && m_cursorPos.x > 0.f && !isSpecialChar)
			{
				// Segments may be empty if first word needed to be wrapped here
				if (!m_segments.empty())
					m_linePositions.push_back(m_cursorPos);
				m_cursorPos.x = 0.f;
				m_cursorPos.y += m_lineMaxCharSize + m_lineSpacing;
				m_lineMaxCharSize = text.getCharacterSize();
			}

			text.setPosition(m_cursorPos);
			m_cursorPos.x += text.getLocalBounds().width;

			m_bounds.width = std::max(m_bounds.width, m_cursorPos.x);
			m_bounds.height = std::max(m_bounds.height, m_cursorPos.y + m_lineMaxCharSize + m_lineSpacing);
			auto bounds = sf::FloatRect(
						text.getGlobalBounds().left - padding,
						text.getGlobalBounds().top - padding,
						text.getLocalBounds().width + padding * 2,
						text.getLocalBounds().height + padding * 2);

			debugShape.setSize(sf::Vector2f(bounds.width, bounds.height));
			debugShape.setPosition(bounds.left, bounds.top);
			m_debugSegmentShapes.push_back(debugShape);

			m_segments.push_back({objectExists, objectId, text, bounds});
		}
	}
	m_linePositions.push_back(m_cursorPos);

	applyAlpha();
	applyHighlightFactor();

	m_debugBorder.setFillColor(sf::Color::Transparent);
	m_debugBorder.setOutlineColor(sf::Color::Red);
	m_debugBorder.setOutlineThickness(2.f);
	m_debugBorder.setSize(sf::Vector2f(m_bounds.width, m_bounds.height - m_bounds.top));
	m_debugBorder.setPosition(m_bounds.left, m_bounds.top);

	m_needsUpdate = false;
}

} // namespace NovelTea