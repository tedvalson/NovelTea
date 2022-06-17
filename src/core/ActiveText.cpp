#include <NovelTea/ActiveText.hpp>
#include <NovelTea/ActiveTextSegment.hpp>
#include <NovelTea/BBCodeParser.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/StringUtils.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

ActiveText::ActiveText()
: m_alpha(255.f)
, m_fontSizeMultiplier(1.f)
, m_highlightFactor(1.f)
, m_lineSpacing(5.f)
, m_isComplete(true)
, m_isWaitingForClick(false)
, m_skipWaitingForClick(false)
, m_currentSegment(nullptr)
, m_callback(nullptr)
{
	m_segmentIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_lineMaxCharSize = 0;
}

ActiveText::ActiveText(const std::string &text)
: ActiveText(text, AnimationProperties())
{
}

ActiveText::ActiveText(const std::string &text, const AnimationProperties &animDefault)
: ActiveText()
{
	setText(text, animDefault);
}

json ActiveText::toJson() const
{
	return sj::Array(
		0,
		m_text
	);
}

bool ActiveText::fromJson(const json &j)
{
	setText(j[1].ToString());
	return true;
}

void ActiveText::reset(bool preservePosition)
{
	auto timePassed = m_timePassed;

	m_currentSegment = nullptr;
	m_isComplete = m_segments.empty();
	m_isWaitingForClick = false;
	m_segmentIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();
	m_lineMaxCharSize = 0;

	m_segmentsActive.clear();
	m_tweenManager.killAll();

	// Compute cursorEnd
	sf::Vector2f cursorPos = m_cursorStart;
	for (auto& segment: m_segments) {
		segment->setFontSizeMultiplier(m_fontSizeMultiplier);
		segment->setSize(m_size);
		segment->setCursorStart(cursorPos);
		segment->setLastLineMaxHeight(m_lineMaxCharSize);
		cursorPos = segment->getCursorEnd();
		m_lineMaxCharSize = segment->getCurrentLineMaxHeight();
	}
	m_cursorEnd = cursorPos;

	// Restore previous position when necessary
	if (preservePosition) {
		auto skipWaiting = m_skipWaitingForClick;
		m_skipWaitingForClick = true;

		addSegmentToQueue(0);
		update(timePassed.asSeconds());

		m_skipWaitingForClick = skipWaiting;
	} else
		addSegmentToQueue(0);

	m_tweenManager.update(0.001f);
}

void ActiveText::setText(const std::string &text)
{
	setText(text, TextProperties(), AnimationProperties());
}

void ActiveText::setText(const std::string &text, const AnimationProperties &animProps)
{
	setText(text, TextProperties(), animProps);
}

void ActiveText::setText(const std::string &text, const TextProperties &textProps)
{
	setText(text, textProps, AnimationProperties());
}

void ActiveText::setText(const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps)
{
	m_text = text;
	m_segments.clear();
	auto t = ActiveGame->getScriptManager()->evalExpressions(text);
	std::cout << "setText: " << t << std::endl;
	std::vector<std::shared_ptr<StyledSegment>> segGroup;
	auto s = BBCodeParser::makeSegments(t, textProps, animProps);
	for (auto &ss : s) {
		if (ss->newGroup && !segGroup.empty()) {
			m_segments.emplace_back(new ActiveTextSegment(segGroup));
			segGroup.clear();
		}
		segGroup.push_back(ss);
	}

	if (!segGroup.empty())
		m_segments.emplace_back(new ActiveTextSegment(segGroup));

	reset();
}

const std::string &ActiveText::getText() const
{
	return m_text;
}

void ActiveText::setSize(const sf::Vector2f &size)
{
	m_size = size;
	reset(true);
}

sf::Vector2f ActiveText::getSize() const
{
	return m_size;
}

void ActiveText::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
	reset(true);
}

float ActiveText::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
}

sf::FloatRect ActiveText::getLocalBounds() const
{
	sf::FloatRect bounds;
	if (m_currentSegment)
		bounds = m_currentSegment->getLocalBounds();
	return bounds;
}

sf::FloatRect ActiveText::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

void ActiveText::setCursorStart(const sf::Vector2f &cursorPos)
{
	m_cursorStart = cursorPos;
	reset(true);
}

const sf::Vector2f &ActiveText::getCursorPosition() const
{
	return m_cursorPos;
}

const sf::Vector2f &ActiveText::getCursorEnd() const
{
	return m_cursorEnd;
}

void ActiveText::setHighlightFactor(float highlightFactor)
{
	m_highlightFactor = highlightFactor;
	for (auto& segment : m_segments)
		segment->setHighlightFactor(highlightFactor);
}

float ActiveText::getHighlightFactor() const
{
	return m_highlightFactor;
}

void ActiveText::setLineSpacing(float lineSpacing)
{
	m_lineSpacing = lineSpacing;
	for (auto& segment : m_segments)
		segment->setLineSpacing(lineSpacing);
}

float ActiveText::getLineSpacing() const
{
	return m_lineSpacing;
}

void ActiveText::setAlpha(float alpha)
{
	m_alpha = alpha;
	for (auto &segment : m_segments)
		segment->setAlpha(alpha);
}

float ActiveText::getAlpha() const
{
	return m_alpha;
}

bool ActiveText::update(float delta)
{
	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
		if (isComplete())
			break;
		if (m_isWaitingForClick && !m_skipWaitingForClick)
			break;
		if (m_segmentIndex < 0 || m_segmentIndex >= m_segments.size())
			break;

		timeDelta -= m_timeToNext;
		m_timePassed += m_timeToNext;
		m_tweenManager.update(m_timeToNext.asSeconds());
	}

	if (!m_isWaitingForClick || m_skipWaitingForClick) {
		m_timePassed += timeDelta;
		m_timeToNext -= timeDelta;
	}
	m_tweenManager.update(timeDelta.asSeconds());
	return Hideable::update(delta);
}

std::string ActiveText::toPlainText(bool stripBBCodes, const std::string &newlineChar) const
{
	std::string result;
	for (auto& segment : m_segments)
		result += segment->toPlainText(stripBBCodes, newlineChar);
	return result;
}

std::string ActiveText::objectFromPoint(const sf::Vector2f &point) const
{
	std::string result;
	for (auto &segment : m_segments) {
		result = segment->objectFromPoint(point);
		if (!result.empty())
			break;
	}
	return result;
}

void ActiveText::setHighlightId(const std::string &id)
{
	for (auto &seg : m_segments)
		seg->setHighlightId(id);
}

bool ActiveText::isComplete() const
{
	return m_isComplete;
}

bool ActiveText::isWaitingForClick() const
{
	return m_isWaitingForClick;
}

void ActiveText::click()
{
	if (m_isWaitingForClick) {
		m_isWaitingForClick = false;
		addSegmentToQueue(m_segmentIndex + 1);
	} else {
		if (m_currentSegment && m_currentSegment->getAnimProps().skippable)
			update(m_timeToNext.asSeconds());
	}
}

const sf::Time &ActiveText::getTimeToNext() const
{
	return m_timeToNext;
}

const SharedVector<ActiveTextSegment> &ActiveText::getSegments() const
{
	return m_segments;
}

size_t ActiveText::getDurationMs() const
{
	if (m_segments.empty())
		return 0;
	else
		return getDurationMs(m_segments.size());
}

size_t ActiveText::getDurationMs(size_t indexEnd) const
{
	auto duration = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		duration += m_segments[i]->getDurationMs();
	return duration;
}

size_t ActiveText::getDelayMs() const
{
	if (m_segments.empty())
		return 0;
	else
		return getDelayMs(m_segments.size());
}

size_t ActiveText::getDelayMs(size_t indexEnd) const
{
	auto delay = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		delay += m_segments[i]->getDelayMs();
	return delay;
}

void ActiveText::onComplete(ActiveTextCallback callback)
{
	m_callback = callback;
}

void ActiveText::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	for (auto &segment : m_segmentsActive) {
		target.draw(*segment, states);
	}
}

void ActiveText::addSegmentToQueue(size_t segmentIndex)
{
	m_segmentIndex = segmentIndex;
	if (segmentIndex >= m_segments.size())
		return;

	auto& segment = m_segments[segmentIndex];
	m_currentSegment = segment;

	TweenEngine::TweenCallbackFunction endCallback = nullptr;
	auto delayMs = segment->getDelayMs();
	auto timeToNext = 0.001f * (delayMs > 0 ? delayMs - 1 : 0);

	m_timeToNext = sf::milliseconds(segment->getDelayMs());
	startTextEffect(segment);

	endCallback = [this, segment, segmentIndex](TweenEngine::BaseTween*)
	{
		if (segmentIndex + 1 >= m_segments.size()) {
			m_isComplete = true;
			if (m_callback)
				m_callback();
		}
		else {
			auto nextSegment = m_segments[segmentIndex+1];
			if (m_skipWaitingForClick || !nextSegment->getAnimProps().waitForClick)
				addSegmentToQueue(segmentIndex + 1);
			else if (!m_skipWaitingForClick) {
				m_isWaitingForClick = true;
			}
		}
	};

	if (endCallback)
		TweenEngine::Tween::mark()
			.delay(timeToNext)
			.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
			.start(m_tweenManager);
}

void ActiveText::startTextEffect(const std::shared_ptr<ActiveTextSegment> &segment)
{
	auto& anim = segment->getAnimProps();
	auto duration = 0.001f * segment->getDurationMs();

	// Push activeText in callback so it doesn't show before update()
	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this, segment](TweenEngine::BaseTween*){
			m_segmentsActive.push_back(segment);
		}).start(m_tweenManager);

	if (anim.type == TextEffect::Fade) {
		segment->setAlpha(0.f);
		TweenEngine::Tween::to(*segment, ActiveTextSegment::ALPHA, duration / anim.speed)
			.target(255.f)
			.start(m_tweenManager);
	}
	else if (anim.type == TextEffect::FadeAcross) {
		segment->setFadeAcrossPosition(0.f);
		TweenEngine::Tween::to(*segment, ActiveTextSegment::FADEACROSS, duration / anim.speed)
			.ease(TweenEngine::TweenEquations::easeInOutLinear)
			.target(1.f)
			.start(m_tweenManager);
	}
}

} // namespace NovelTea
