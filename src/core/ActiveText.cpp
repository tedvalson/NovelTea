#include <NovelTea/ActiveText.hpp>
#include <NovelTea/ActiveTextSegment.hpp>
#include <NovelTea/BBCodeParser.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/StringUtils.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

ActiveText::ActiveText()
: m_alpha(255.f)
, m_fontSizeMultiplier(1.f)
, m_highlightFactor(1.f)
, m_lineSpacing(5.f)
, m_isAnimating(true)
, m_isComplete(true)
, m_isWaitingForClick(false)
, m_skipWaitingForClick(false)
, m_currentSegment(nullptr)
, m_callback(nullptr)
{
	m_segmentIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
}

ActiveText::ActiveText(const std::string &text)
: ActiveText(text, TextProperties(), AnimationProperties())
{}

ActiveText::ActiveText(const std::string &text, const AnimationProperties &animProps)
: ActiveText(text, TextProperties(), animProps)
{}

ActiveText::ActiveText(const std::string &text, const TextProperties &textProps)
: ActiveText(text, textProps, AnimationProperties())
{}

ActiveText::ActiveText(const std::string &text, const TextProperties &textProps, const AnimationProperties &animProps)
: ActiveText()
{
	setText(text, textProps, animProps);
}

void ActiveText::reset(bool preservePosition)
{
	auto timePassed = m_timePassed;

	m_currentSegment = nullptr;
	m_isComplete = m_segments.empty();
	m_isAnimating = !m_isComplete;
	m_isWaitingForClick = false;
	m_segmentIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();
	int lineMaxCharSize = 0;

	m_segmentsActive.clear();
	m_tweenManager.killAll();

	// Compute cursorEnd
	sf::Vector2f cursorPos = m_cursorStart;
	for (auto& segment: m_segments) {
		segment->reset();
		segment->setFontSizeMultiplier(m_fontSizeMultiplier);
		segment->setSize(m_size);
		segment->setCursorStart(cursorPos);
		segment->setLastLineMaxHeight(lineMaxCharSize);
		cursorPos = segment->getCursorEnd();
		lineMaxCharSize = segment->getCurrentLineMaxHeight();
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

	setAlpha(m_alpha);
}

void ActiveText::skipToNext(bool skipWaitForClick)
{
	if (m_currentSegment && !m_currentSegment->getAnimProps().skippable)
		return;
	update(m_timeToNext.asSeconds());
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
	m_text = ActiveGame->getScriptManager()->evalExpressions(text);
	buildSegments(textProps, animProps);
	reset();
}

const std::string &ActiveText::getText() const
{
	return m_text;
}

void ActiveText::updateProps(const TextProperties &textProps, const AnimationProperties &animProps)
{
	buildSegments(textProps, animProps);
	reset(true);
}

void ActiveText::updateProps(const TextProperties &textProps)
{
	updateProps(textProps, AnimationProperties());
}

void ActiveText::show(float duration, int tweenType, HideableCallback callback)
{
	Hideable::show(duration, tweenType, callback);
}

void ActiveText::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, callback);
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
	for (auto &segment : m_segmentsActive)
		segment->setAlpha(alpha);
}

float ActiveText::getAlpha() const
{
	return m_alpha;
}

bool ActiveText::update(float delta)
{

	auto timeDelta = sf::seconds(delta);
	while (timeDelta >= m_timeToNext && m_timeToNext > sf::Time::Zero)
	{
		if (isComplete())
			break;
		if (m_isWaitingForClick && !m_skipWaitingForClick)
			break;
		if (m_segmentIndex < 0 || m_segmentIndex >= m_segments.size())
			break;

		timeDelta -= m_timeToNext;
		m_timePassed += m_timeToNext;
		updateSegments(m_timeToNext.asSeconds());
		m_tweenManager.update(m_timeToNext.asSeconds());
	}

	if (!m_isWaitingForClick || m_skipWaitingForClick) {
		m_timePassed += timeDelta;
		m_timeToNext -= timeDelta;
	}
	updateSegments(timeDelta.asSeconds());
	m_tweenManager.update(timeDelta.asSeconds());
	return Hideable::update(delta);
}

std::string ActiveText::toPlainText(bool stripBBCodes, const std::string &newlineChar) const
{
	std::string result = replace(m_text, "\n", newlineChar);
	return result;
}

std::string ActiveText::objectFromPoint(const sf::Vector2f &point) const
{
	std::string result;
	auto p = getTransform().getInverse().transformPoint(point);
	for (auto &segment : m_segments) {
		result = segment->objectFromPoint(p);
		if (!result.empty())
			break;
	}
	return result;
}

void ActiveText::setHighlightId(const std::string &id)
{
	m_highlightId = id;
	for (auto &seg : m_segments)
		seg->setHighlightId(id);
}

bool ActiveText::isAnimating() const
{
	return m_isAnimating;
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
		skipToNext(m_skipWaitingForClick);
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

void ActiveText::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case HIGHLIGHTS:
			setHighlightFactor(newValues[0]);
			break;
		default:
			Hideable::setValues(tweenType, newValues);
	}
}

int ActiveText::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case HIGHLIGHTS:
			returnValues[0] = getHighlightFactor();
			return 1;
		default:
			return Hideable::getValues(tweenType, returnValues);
	}
}

void ActiveText::addSegmentToQueue(size_t segmentIndex)
{
	m_segmentIndex = segmentIndex;
	m_timeToNext = sf::Time::Zero;
	if (segmentIndex >= m_segments.size()) {
		m_currentSegment = nullptr;
		return;
	}

	auto& segment = m_segments[segmentIndex];
	m_currentSegment = segment;

	TweenEngine::TweenCallbackFunction endCallback = nullptr;
	auto delayMs = segment->getDelayMs();
	auto timeToNext = 0.001f * (delayMs > 1 ? delayMs : 1);

	m_timeToNext = sf::seconds(timeToNext);
	m_segmentsActive.push_back(segment);
	segment->startAnim();

	endCallback = [this, segment, segmentIndex](TweenEngine::BaseTween*)
	{
		auto nextIndex = segmentIndex + 1;
		if (nextIndex >= m_segments.size()) {
			m_isComplete = true;
			if (m_callback)
				m_callback();
		}
		else {
			if (m_skipWaitingForClick || !segment->getAnimProps().waitForClick)
				addSegmentToQueue(nextIndex);
			else if (!m_skipWaitingForClick) {
				m_isWaitingForClick = true;
			}
		}
	};

		TweenEngine::Tween::mark()
			.delay(timeToNext)
			.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
			.start(m_tweenManager);
}

void ActiveText::buildSegments(const TextProperties &textProps, const AnimationProperties &animProps)
{
	m_segments.clear();
	std::vector<std::shared_ptr<StyledSegment>> segGroup;
	auto s = BBCodeParser::makeSegments(m_text, textProps, animProps);
	for (auto &ss : s) {
		if (ss->newGroup && !segGroup.empty()) {
			m_segments.emplace_back(new ActiveTextSegment(segGroup));
			segGroup.clear();
		}
		segGroup.push_back(ss);
	}

	if (!segGroup.empty())
		m_segments.emplace_back(new ActiveTextSegment(segGroup));

	setHighlightFactor(m_highlightFactor);
	setHighlightId(m_highlightId);
	setLineSpacing(m_lineSpacing);
	setAlpha(m_alpha);
}

void ActiveText::updateSegments(float delta)
{
	m_isAnimating = m_tweenManager.getRunningTweensCount() > 0;
	for (auto& segment : m_segmentsActive)
		if (segment->update(delta))
			m_isAnimating = true;
}

} // namespace NovelTea
