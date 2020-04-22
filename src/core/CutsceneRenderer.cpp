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
	m_callbacksQueued = 0;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();

	m_texts.clear();
	m_tweenManager.killAll();

	addSegmentToQueue(0);
}

void CutsceneRenderer::update(float delta)
{
	auto segments = m_cutscene->segments();
	if (m_segmentIndex >= segments.size())
		return;

	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
		if (m_segmentIndex >= segments.size())
			break;

		unsigned int segmentIndex;
		timeDelta -= m_timeToNext;
		m_tweenManager.update(m_timeToNext.asSeconds());

		do
		{
			segmentIndex = m_segmentIndex;
			m_tweenManager.update(0.f);
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
	{
		target.draw(*text, states);
	}
}

void CutsceneRenderer::addSegmentToQueue(unsigned int segmentIndex)
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
		timeToNext = 0.001f * seg->getTransitionDuration();

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			auto activeText = seg->getActiveText();
			activeText->setCursorStart(m_cursorPos);
			activeText->setPosition(0.f, 0.f);
			m_cursorPos = activeText->getCursorEnd();
			m_timeToNext = sf::milliseconds(seg->getTransitionDuration());
			m_texts.push_back(activeText);
		};
		endCallback = [this, segmentIndex](TweenEngine::BaseTween*)
		{
			addSegmentToQueue(segmentIndex + 1);
		};
	}
	else if (type == CutsceneSegment::PageBreak)
	{
		auto seg = static_cast<CutscenePageBreakSegment*>(segment.get());
		timeToNext = 0.001f * seg->getTransitionDuration();

		beginCallback = [this, seg, timeToNext](TweenEngine::BaseTween*)
		{
			m_timeToNext = sf::milliseconds(seg->getTransitionDuration());

			for (auto &text : m_texts)
			{
				TweenEngine::Tween::to(*text, ActiveText::POSITION_X, timeToNext)
					.target(0.f - text->getSize().x)
					.start(m_tweenManager);
			}
		};

		endCallback = [this, segmentIndex](TweenEngine::BaseTween*) {
			m_texts.clear();
			m_cursorPos = sf::Vector2f();
			addSegmentToQueue(segmentIndex + 1);
		};
	}
	else
	{
		// TODO: Throw error
	}

	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, beginCallback)
		.start(m_tweenManager);
	TweenEngine::Tween::mark()
		.delay(timeToNext)
		.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
		.start(m_tweenManager);
}

} // namespace NovelTea
