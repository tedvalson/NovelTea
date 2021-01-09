#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/AssetManager.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/Graphics/RenderTarget.hpp>

namespace NovelTea
{

CutsceneRenderer::CutsceneRenderer()
: m_skipWaitingForClick(false)
, m_size(400.f, 400.f)
, m_margin(10.f)
{
	auto fadeTexture = AssetManager<sf::Texture>::get("images/fade.png").get();
	m_fadeRectTop.setTexture(fadeTexture);
	m_fadeRectTop.setFillColor(sf::Color(200,200,200));
	m_fadeRectBottom = m_fadeRectTop;
	m_fadeRectTop.setRotation(180.f);

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	auto &text = m_icon.getText();
	text.setString(L"\uf138");
	text.setFillColor(sf::Color(120, 120, 120, 0));

	setCutscene(std::make_shared<Cutscene>());
}

void CutsceneRenderer::setCutscene(const std::shared_ptr<Cutscene> &cutscene)
{
	m_cutscene = cutscene;
	reset();
}

void CutsceneRenderer::reset(bool preservePosition)
{
	auto timePassed = m_timePassed;

	m_currentSegment = nullptr;
	m_isComplete = false;
	m_isWaitingForClick = false;
	m_segmentIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();
	m_scrollPos = 0.f;

	m_texts.clear();
	m_textsOld.clear();
	m_icon.hide(0.f);
	m_tweenManager.killAll();

	if (preservePosition) {
		auto skipWaiting = m_skipWaitingForClick;
		m_skipWaitingForClick = true;

		if (timePassed.asSeconds() > 0.2f)
			timePassed -= sf::seconds(0.2f);

		addSegmentToQueue(0);
		update(timePassed.asSeconds());

		m_skipWaitingForClick = skipWaiting;
	} else
		addSegmentToQueue(0);
}

bool CutsceneRenderer::processEvent(const sf::Event &event)
{
	return m_scrollBar.processEvent(event);
}

void CutsceneRenderer::update(float delta)
{
	auto segments = m_cutscene->segments();

	m_icon.update(delta);
	m_scrollBar.update(delta);
	m_tweenManager.update(0.001f); // Trigger next segment if delay is 0

	delta *= m_cutscene->getSpeedFactor();
	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
		if (m_isWaitingForClick && !m_skipWaitingForClick)
			break;
		if (m_segmentIndex < 0 || m_segmentIndex >= segments.size())
			break;

		size_t segmentIndex;
		timeDelta -= m_timeToNext;
		m_timePassed += m_timeToNext;
		m_tweenManager.update(m_timeToNext.asSeconds());

		do
		{
			segmentIndex = m_segmentIndex;
			m_tweenManager.update(0.001f); // Trigger next segment if delay is 0
			m_tweenManager.update(0.001f); // Trigger possible 0 delay tween in above segment
		}
		while (segmentIndex != m_segmentIndex);
	}

	if (!m_isWaitingForClick || m_skipWaitingForClick) {
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
		m_isWaitingForClick = false;
		m_icon.hide(0.4f);
		addSegmentToQueue(m_segmentIndex + 1);
	} else {
		if (m_currentSegment && m_currentSegment->getCanSkip())
			update(m_timeToNext.asSeconds());
	}
}

void CutsceneRenderer::setScrollTween(float position, float duration)
{
	float targetPos;
	float minPos = m_size.y - m_margin*2 - m_scrollAreaSize.y;
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
		TweenEngine::Tween::to(m_scrollBar, ScrollBar::SCROLLPOS, duration)
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

	m_fadeRectBottom.setSize(sf::Vector2f(998.f, m_margin));
	m_fadeRectTop.setSize(sf::Vector2f(998.f, m_margin));
	m_fadeRectTop.setOrigin(m_fadeRectTop.getSize());
	m_fadeRectBottom.setPosition(0.f, m_size.y - m_margin);
}

void CutsceneRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
	reset(true);
}

const sf::Vector2f &CutsceneRenderer::getSize() const
{
	return m_size;
}

void CutsceneRenderer::setMargin(float margin)
{
	m_margin = margin;
	reset(true);
}

float CutsceneRenderer::getMargin() const
{
	return m_margin;
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

	target.draw(m_fadeRectTop);
	target.draw(m_fadeRectBottom);
	target.draw(m_scrollBar);
	target.draw(m_icon);
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

	activeText->setPosition((m_size.x < m_size.y ? 0.f : 0.2f * m_size.x), 0.f);

	if (effect == CutsceneTextSegment::Fade) {
		activeText->setAlpha(0.f);
		TweenEngine::Tween::to(*activeText, ActiveText::ALPHA, duration)
			.target(255.f)
			.start(m_tweenManager);
	}
	else if (effect == CutsceneTextSegment::FadeAcross) {
		activeText->setFadeAcrossPosition(0.f);
		TweenEngine::Tween::to(*activeText, ActiveText::FADEACROSS, duration)
			.ease(TweenEngine::TweenEquations::easeInOutLinear)
			.target(1.f)
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
	m_scrollBar.setPosition(m_size.x - 4.f, 4.f);
	m_scrollBar.setSize(sf::Vector2u(2, m_size.y - 8.f));
	m_scrollBar.setScrollAreaSize(sf::Vector2u(m_size.x, m_size.y - m_margin*2));

	m_segmentIndex = segmentIndex;
	auto segments = m_cutscene->segments();
	if (segmentIndex >= segments.size())
		return;

	auto segment = segments[segmentIndex];
	m_currentSegment = segment;
	auto type = segment->type();

	TweenEngine::TweenCallbackFunction beginCallback, endCallback;
	auto timeToNext = 0.f;

	if (type == CutsceneSegment::Text)
	{
		auto seg = static_cast<CutsceneTextSegment*>(segment.get());
		timeToNext = 0.001f * seg->getDelay();

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			// This gives some padding at the bottom for text
			auto scrollAreaMargin = m_margin * 2;

			auto activeText = seg->getActiveText();
			activeText->setSize(sf::Vector2f((m_size.x < m_size.y ? 1.f : 0.6f) * m_size.x - m_margin*2, m_size.y));
			if (seg->getBeginWithNewLine()) {
				m_cursorPos.x = 0.f;
				m_cursorPos.y = m_scrollAreaSize.y - scrollAreaMargin;
			}
			m_cursorPos.x += seg->getOffsetX();
			m_cursorPos.y += seg->getOffsetY();

			activeText->setCursorStart(m_cursorPos);
			m_cursorPos = activeText->getCursorEnd();
			m_timeToNext = sf::milliseconds(seg->getDelay());
			startTransitionEffect(seg);

			m_scrollAreaSize.y = activeText->getLocalBounds().height + scrollAreaMargin;
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
			m_scrollAreaSize.y = m_margin * 2;
			startTransitionEffect(seg);
		};
	}
	else
	{
		// TODO: Throw error
	}

	endCallback = [this, segment, segmentIndex](TweenEngine::BaseTween*)
	{
		if (segmentIndex + 1 >= m_cutscene->segments().size())
			m_isComplete = true;
		else {
			auto nextSegment = m_cutscene->segments()[segmentIndex+1];
			if (m_skipWaitingForClick || !nextSegment->getWaitForClick())
				addSegmentToQueue(segmentIndex + 1);
			else if (!m_skipWaitingForClick) {
				m_isWaitingForClick = true;
				auto posY = std::min(m_scrollAreaSize.y, m_size.y - m_margin * 2);
				m_icon.setPosition(m_size.x / 2, posY);
				m_icon.show(2.f);
			}
		}
	};

	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, beginCallback)
		.start(m_tweenManager);
	TweenEngine::Tween::mark()
		.delay(timeToNext)
		.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
		.start(m_tweenManager);
	repositionItems();
}

} // namespace NovelTea
