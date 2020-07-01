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
	m_segmentIndex = 0;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();

	m_texts.clear();
	m_textsOld.clear();
	m_tweenManager.killAll();

	addSegmentToQueue(0);
}

void CutsceneRenderer::update(float delta)
{
	auto segments = m_cutscene->segments();
	if (m_segmentIndex >= segments.size())
	{
		m_tweenManager.update(delta);
		return;
	}

	delta *= m_cutscene->getSpeedFactor();
	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
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

	m_timePassed += timeDelta;
	m_timeToNext -= timeDelta;
	m_tweenManager.update(timeDelta.asSeconds());
}

void CutsceneRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	for (auto &text : m_texts)
		target.draw(*text, states);
	for (auto &text : m_textsOld)
		target.draw(*text, states);
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
	TweenEngine::Tween::set(*activeText, ActiveText::ALPHA)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(*activeText, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
}

void CutsceneRenderer::startTransitionEffect(const CutscenePageBreakSegment *segment)
{
	auto effect = segment->getTransition();
	auto duration = 0.001f * segment->getDuration();

	for (auto &text : m_textsOld)
	{
		TweenEngine::Tween::to(*text, ActiveText::POSITION_X, duration)
			.target(0.f - text->getSize().x)
			.start(m_tweenManager);
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
			activeText->setCursorStart(m_cursorPos);
			m_cursorPos = activeText->getCursorEnd();
			m_timeToNext = sf::milliseconds(seg->getDelay());
			startTransitionEffect(seg);
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
			m_timeToNext = sf::milliseconds(seg->getDelay());
			m_cursorPos = sf::Vector2f();
			startTransitionEffect(seg);
		};
	}
	else
	{
		// TODO: Throw error
	}

	endCallback = [this, segmentIndex](TweenEngine::BaseTween*)
	{
		addSegmentToQueue(segmentIndex + 1);
	};

	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, beginCallback)
		.start(m_tweenManager);
	TweenEngine::Tween::mark()
		.delay(timeToNext)
		.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
		.start(m_tweenManager);
}

} // namespace NovelTea
