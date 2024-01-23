#include <NovelTea/CutscenePlayer.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutsceneScriptSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>
//#include <NovelTea/Err.hpp>
#include <TweenEngine/Tween.h>
//#include <algorithm>

namespace NovelTea {

CutscenePlayer::CutscenePlayer(Context *context)
: ContextObject(context)
, m_skipWaitingForClick(false)
, m_skipScriptSegments(false)
, m_skipConditionChecks(false)
{
	setCutscene(std::make_shared<Cutscene>(getContext()));
}

CutscenePlayer::~CutscenePlayer()
{
}

void CutscenePlayer::setCutscene(const std::shared_ptr<Cutscene> &cutscene)
{
	m_cutscene = cutscene;
	reset();
}

const std::shared_ptr<Cutscene> &CutscenePlayer::getCutscene() const
{
	return m_cutscene;
}

void CutscenePlayer::reset(bool preservePosition)
{
	auto timePassed = m_timePassed;

	m_currentSegment = nullptr;
	m_isComplete = false;
	m_isWaitingForClick = false;
	m_segmentIndex = -1;
	m_segmentSaveIndex = -1;
	m_timePassed = 0;
	m_timeToNext = 0;

	m_cutscene->setSkipConditionChecks(m_skipConditionChecks);
	ScriptMan->setActiveEntity(m_cutscene);

	if (preservePosition) {
		auto skipScripts = m_skipScriptSegments;
		auto skipWaiting = m_skipWaitingForClick;
		m_skipScriptSegments = true;
		m_skipWaitingForClick = true;

		addSegmentToQueue(0);
		update(timePassed);

		m_skipScriptSegments = skipScripts;
		m_skipWaitingForClick = skipWaiting;
	} else
		addSegmentToQueue(0);
}

void CutscenePlayer::update(float delta)
{
	auto segments = m_cutscene->segments();

	delta *= m_cutscene->getSpeedFactor();
//	auto timeDelta = sf::seconds(delta);
	auto timeDelta = delta;

	while (timeDelta >= m_timeToNext)
	{
		if (m_isWaitingForClick && !m_skipWaitingForClick)
			break;
		if (m_segmentIndex < 0 || m_segmentIndex >= segments.size())
			break;

		timeDelta -= m_timeToNext;
		m_timePassed += m_timeToNext;
//		m_tweenManager.update(m_timeToNext.asSeconds());
//		for (auto& text : m_texts)
//			text->update(m_timeToNext.asSeconds());
	}

	if (!m_isWaitingForClick || m_skipWaitingForClick) {
		m_timePassed += timeDelta;
		m_timeToNext -= timeDelta;
	}
	m_tweenManager.update(timeDelta);
//	for (auto& text : m_texts)
//		text->update(timeDelta.asSeconds());
}

bool CutscenePlayer::isComplete() const
{
	return m_isComplete;
}

bool CutscenePlayer::isWaitingForClick() const
{
	return m_isWaitingForClick;
}

void CutscenePlayer::click()
{
	if (m_isWaitingForClick) {
		m_isWaitingForClick = false;
		addSegmentToQueue(m_segmentIndex + 1);
	} else {
		if (m_currentSegment && m_currentSegment->getCanSkip())
			update(m_timeToNext);
	}
}

sj::JSON CutscenePlayer::saveState() const
{
	auto index = getInternalSegmentIndex(m_segmentSaveIndex);
	if (index < 0)
		index = m_cutscene->internalSegments().size();
	return sj::Array(index);
}

void CutscenePlayer::restoreState(const sj::JSON &jstate)
{
	reset();
	auto segmentIndex = jstate[0].ToInt();
	if (segmentIndex < 1 || segmentIndex > m_cutscene->internalSegments().size() + 1)
		return;
	auto timeMs = m_cutscene->getDelayMs(segmentIndex) - 2;
	setSkipScriptSegments(true);
	setSkipWaitingForClick(true);
	update(0.001f * timeMs);
	setSkipScriptSegments(false);
	setSkipWaitingForClick(false);
}

int CutscenePlayer::getSegmentIndex() const
{
	return m_segmentIndex;
}

int CutscenePlayer::getInternalSegmentIndex(int index) const
{
	int pos = (index < 0) ? m_segmentIndex : index;
	for (int i = 0; i < m_cutscene->internalSegments().size(); ++i)
	{
		auto &segment = m_cutscene->internalSegments()[i];
		if (segment->type() == CutsceneSegment::Page) {
			auto pageSegment = static_cast<CutscenePageSegment*>(segment.get());
			pos -= pageSegment->getSegmentCount();
		} else
			--pos;
		if (pos < 0)
			return i;
	}
	return -1;
}

void CutscenePlayer::addSegmentToQueue(size_t segmentIndex)
{
	m_segmentIndex = segmentIndex;
	m_segmentSaveIndex = segmentIndex;
	auto segments = m_cutscene->segments();
	if (segmentIndex >= segments.size())
		return;

	auto segment = segments[segmentIndex];
	m_currentSegment = segment;
	auto type = segment->type();

	// Move to next segment if condition check fails
	if (!m_skipConditionChecks && !segment->conditionPasses()) {
		addSegmentToQueue(m_segmentIndex + 1);
		return;
	}

	TweenEngine::TweenCallbackFunction beginCallback = nullptr;
	TweenEngine::TweenCallbackFunction endCallback = nullptr;
	auto delayMs = segment->getFullDelay();
	auto timeToNext = 0.001f * (delayMs > 0 ? delayMs - 1 : 0);

	if (type == CutsceneSegment::Text)
	{
		auto seg = static_cast<CutsceneTextSegment*>(segment.get());

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
//			auto activeText = ActiveText::fromCutsceneTextSegment(seg);
//			activeText->reset();
//			activeText->setSkipWaitingForClick(m_skipWaitingForClick);
//			activeText->setSkipWaitingForClick(true); // TODO: set this right
			if (seg->getBeginWithNewLine()) {
				std::cout << std::endl;
			}

			std::cout << seg->getText();

			m_timeToNext = 0.001f * seg->getFullDelay();
//			startTransitionEffect(seg);
		};
	}
	else if (type == CutsceneSegment::PageBreak)
	{
		auto seg = static_cast<CutscenePageBreakSegment*>(segment.get());

		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
//			m_textsOld = m_texts;
//			m_texts.clear();
			m_timeToNext = 0.001f * seg->getDelay();
//			startTransitionEffect(seg);
		};
	}
	else if (type == CutsceneSegment::Script)
	{
		auto seg = static_cast<CutsceneScriptSegment*>(segment.get());
		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			m_timeToNext = 0.001f * seg->getDelay();
			if (m_skipScriptSegments)
				return;

			if (seg->getAutosaveBefore()) {
				m_segmentSaveIndex = m_segmentIndex;
				GGame->autosave();
			}

			seg->runScript(m_cutscene);

			if (seg->getAutosaveAfter()) {
				m_segmentSaveIndex = m_segmentIndex + 1;
				GGame->autosave();
			}
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
			if (m_skipWaitingForClick || !segment->getWaitForClick())
				addSegmentToQueue(segmentIndex + 1);
			else if (!m_skipWaitingForClick) {
				waitForClick();
			}
		}
	};

	if (beginCallback)
		beginCallback(nullptr);
	if (endCallback)
		TweenEngine::Tween::mark()
			.delay(timeToNext)
			.setCallback(TweenEngine::TweenCallback::BEGIN, endCallback)
			.start(m_tweenManager);
}

void CutscenePlayer::waitForClick()
{
	m_isWaitingForClick = true;
}

} // namespace NovelTea
