#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/ActiveText.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/Graphics/RenderTarget.hpp>

namespace NovelTea
{

CutsceneRenderer::CutsceneRenderer()
: m_size(400.f, 400.f)
, m_skipWaitingForClick(false)
, m_margin(10.f)
{
	setCutscene(std::make_shared<Cutscene>());
}

void CutsceneRenderer::setCutscene(const std::shared_ptr<Cutscene> &cutscene)
{
	m_cutscene = cutscene;
	reset();

}

void CutsceneRenderer::reset()
{
	m_isComplete = false;
	m_isWaitingForClick = false;
	m_segmentIndex = 0;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();
	m_scrollPos = 0.f;

	m_texts.clear();
	m_textsOld.clear();
	m_tweenManager.killAll();

	addSegmentToQueue(0);
}

void CutsceneRenderer::update(float delta)
{
	auto segments = m_cutscene->segments();

	delta *= m_cutscene->getSpeedFactor();
	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
		if (m_isWaitingForClick)
			break;
		if (m_segmentIndex >= segments.size())
			break;

		size_t segmentIndex;
		timeDelta -= m_timeToNext;
		m_tweenManager.update(m_timeToNext.asSeconds());

		do
		{
			segmentIndex = m_segmentIndex;
			m_tweenManager.update(0.001f); // Trigger next segment if delay is 0
			m_tweenManager.update(0.001f); // Trigger possible 0 delay tween in above segment
		}
		while (segmentIndex != m_segmentIndex);
	}

	if (!m_isWaitingForClick) {
		m_timePassed += timeDelta;
		m_timeToNext -= timeDelta;
	}
	m_tweenManager.update(timeDelta.asSeconds());
}

bool CutsceneRenderer::isComplete() const
{
	return m_isComplete;
}

bool CutsceneRenderer::isWaitingForClick() const
{
	return m_isWaitingForClick;
}

void CutsceneRenderer::click()
{
	if (m_isWaitingForClick) {
		addSegmentToQueue(m_segmentIndex + 1);
	}
	m_isWaitingForClick = false;
}

void CutsceneRenderer::setScrollTween(float position, float duration)
{
	float targetPos;
	float minPos = m_size.y - m_margin*2 - m_scrollAreaSize.y - 40.f;
	if (minPos > 0.f)
		minPos = 0.f;
	if (position < minPos)
		targetPos = minPos;
	else
		targetPos = position;

	if (duration == 0.f) {
		m_scrollPos = targetPos;
		updateScrollbar();
		repositionItems();
	} else {
		TweenEngine::Tween::to(*this, _SCROLLPOS, duration)
			.target(targetPos)
			.start(m_tweenManager);
	}
}

void CutsceneRenderer::setScroll(float position)
{
	setScrollTween(position, 0.f);
}

float CutsceneRenderer::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &CutsceneRenderer::getScrollSize()
{
	return m_scrollAreaSize;
}

void CutsceneRenderer::repositionItems()
{
	m_scrollTransform = sf::Transform::Identity;
	m_scrollTransform.translate(m_margin, m_margin + round(m_scrollPos));
}

void CutsceneRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	auto textStates = states;
	auto textOldStates = states;
	textStates.transform *= m_scrollTransform;
	textOldStates.transform *= m_scrollTransformOld;

	for (auto &text : m_texts)
		target.draw(*text, textStates);
	for (auto &text : m_textsOld)
		target.draw(*text, textOldStates);
}

void CutsceneRenderer::startTransitionEffect(const CutsceneTextSegment *segment)
{
	auto activeText = segment->getActiveText();
	auto effect = segment->getTransition();
	auto duration = 0.001f * segment->getDuration();

	// Push activeText in callback so it doesn't show before update()
	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this, activeText](TweenEngine::BaseTween*){
			m_texts.push_back(activeText);
		}).start(m_tweenManager);

	activeText->setPosition(0.f, 0.f);
	activeText->setAlpha(0.f);

	if (effect == CutsceneTextSegment::None) {
		activeText->setAlpha(255.f);
	}
	else if (effect == CutsceneTextSegment::Fade) {
		TweenEngine::Tween::to(*activeText, ActiveText::ALPHA, duration)
			.target(255.f)
			.start(m_tweenManager);
	}
}

void CutsceneRenderer::startTransitionEffect(const CutscenePageBreakSegment *segment)
{
	auto effect = segment->getTransition();
	auto duration = 0.001f * segment->getDuration();

	for (auto &text : m_textsOld)
	{
		if (effect == CutscenePageBreakSegment::None) {
			TweenEngine::Tween::set(*text, ActiveText::ALPHA)
				.target(0.f)
				.delay(duration)
				.start(m_tweenManager);
		}
		else if (effect == CutscenePageBreakSegment::Fade) {
			TweenEngine::Tween::to(*text, ActiveText::ALPHA, duration)
				.target(0.f)
				.start(m_tweenManager);
		}
		else if (effect == CutscenePageBreakSegment::ScrollLeft) {
			TweenEngine::Tween::to(*text, ActiveText::POSITION_X, duration)
				.target(-getPosition().x - text->getSize().x - m_margin)
				.start(m_tweenManager);
		}
	}
}

void CutsceneRenderer::addSegmentToQueue(size_t segmentIndex)
{
	m_segmentIndex = segmentIndex;
	auto segments = m_cutscene->segments();
	if (segmentIndex >= segments.size())
		return;

	auto segment = segments[segmentIndex];
	auto type = segment->type();

	TweenEngine::TweenCallbackFunction beginCallback, endCallback;
	auto timeToNext = 0.f;

	if (type == CutsceneSegment::Text)
	{
		auto seg = static_cast<CutsceneTextSegment*>(segment.get());
		timeToNext = 0.001f * seg->getDelay();

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			auto activeText = seg->getActiveText();
			activeText->setSize(sf::Vector2f(m_size.x - m_margin*2, 0.f));
			activeText->setCursorStart(m_cursorPos);
			m_cursorPos = activeText->getCursorEnd();
			m_timeToNext = sf::milliseconds(seg->getDelay());
			startTransitionEffect(seg);
			// TODO: no fixed val
			m_scrollAreaSize.y = m_cursorPos.y + 40.f;
			updateScrollbar();

			if (m_cursorPos.y + 60.f > m_size.y - m_margin*2 - m_scrollPos) {
				setScrollTween(-m_scrollAreaSize.y, 1.f);
			}
			repositionItems();
		};
	}
	else if (type == CutsceneSegment::PageBreak)
	{
		auto seg = static_cast<CutscenePageBreakSegment*>(segment.get());
		timeToNext = 0.001f * seg->getDelay();

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			m_textsOld = m_texts;
			m_texts.clear();
			m_scrollTransformOld = m_scrollTransform;
			setScroll(0.f);
			m_timeToNext = sf::milliseconds(seg->getDelay());
			m_cursorPos = sf::Vector2f();
			startTransitionEffect(seg);
		};
	}
	else
	{
		// TODO: Throw error
	}

	endCallback = [this, segment, segmentIndex](TweenEngine::BaseTween*)
	{
		if (m_skipWaitingForClick || !segment->getWaitForClick())
			addSegmentToQueue(segmentIndex + 1);
		else if (!m_skipWaitingForClick)
			m_isWaitingForClick = true;

		if (segmentIndex + 1 >= m_cutscene->segments().size())
			m_isComplete = true;
	};

	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, beginCallback)
		.start(m_tweenManager);
	TweenEngine::Tween::mark()
		.delay(timeToNext)
		.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
		.start(m_tweenManager);
}

void CutsceneRenderer::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case _SCROLLPOS: {
			m_scrollPos = newValues[0];
			updateScrollbar();
			repositionItems();
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int CutsceneRenderer::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case _SCROLLPOS:
			returnValues[0] = getScroll();
		return 1;
	default:
		return TweenTransformable::getValues(tweenType, returnValues);
	}
}

} // namespace NovelTea
