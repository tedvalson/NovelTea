#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutsceneScriptSegment.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/Graphics/RenderTarget.hpp>

namespace NovelTea
{

CutsceneRenderer::CutsceneRenderer(Context *context)
: ContextObject(context)
, m_skipWaitingForClick(false)
, m_skipScriptSegments(false)
, m_skipConditionChecks(false)
, m_icon(context)
, m_size(400.f, 400.f)
, m_margin(10.f)
, m_fontSizeMultiplier(1.f)
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

	setCutscene(std::make_shared<Cutscene>(getContext()));
}

void CutsceneRenderer::setCutscene(const std::shared_ptr<Cutscene> &cutscene)
{
	m_cutscene = cutscene;
	reset();
}

const std::shared_ptr<Cutscene> &CutsceneRenderer::getCutscene() const
{
	return m_cutscene;
}

void CutsceneRenderer::reset(bool preservePosition)
{
	auto timePassed = m_timePassed;

	m_currentSegment = nullptr;
	m_isComplete = false;
	m_isWaitingForClick = false;
	m_segmentIndex = -1;
	m_segmentSaveIndex = -1;
	m_timePassed = sf::Time::Zero;
	m_timeToNext = sf::Time::Zero;
	m_cursorPos = sf::Vector2f();
	m_scrollPos = 0.f;
	m_scrollAreaSize = sf::Vector2f();

	m_texts.clear();
	m_textsOld.clear();
	m_icon.hide(0.f);
	m_tweenManager.killAll();

	m_cutscene->setSkipConditionChecks(m_skipConditionChecks);

	ScriptMan->setActiveEntity(m_cutscene);

	if (preservePosition) {
		auto skipScripts = m_skipScriptSegments;
		auto skipWaiting = m_skipWaitingForClick;
		m_skipScriptSegments = true;
		m_skipWaitingForClick = true;

		addSegmentToQueue(0);
		update(timePassed.asSeconds());

		m_skipScriptSegments = skipScripts;
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

	delta *= m_cutscene->getSpeedFactor();
	auto timeDelta = sf::seconds(delta);

	while (timeDelta >= m_timeToNext)
	{
		if (m_isWaitingForClick && !m_skipWaitingForClick)
			break;
		if (m_segmentIndex < 0 || m_segmentIndex >= segments.size())
			break;

		timeDelta -= m_timeToNext;
		m_timePassed += m_timeToNext;
		m_tweenManager.update(m_timeToNext.asSeconds());
		for (auto& text : m_texts)
			text->update(m_timeToNext.asSeconds());
	}

	if (!m_isWaitingForClick || m_skipWaitingForClick) {
		m_timePassed += timeDelta;
		m_timeToNext -= timeDelta;
	}
	m_tweenManager.update(timeDelta.asSeconds());
	for (auto& text : m_texts)
		text->update(timeDelta.asSeconds());
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

sj::JSON CutsceneRenderer::saveState() const
{
	auto index = getInternalSegmentIndex(m_segmentSaveIndex);
	if (index < 0)
		index = m_cutscene->internalSegments().size();
	return sj::Array(index);
}

void CutsceneRenderer::restoreState(const sj::JSON &jstate)
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

void CutsceneRenderer::setScrollTween(float position, float duration)
{
	float targetPos;
	float minPos = m_size.y - m_margin*2 - m_scrollAreaSize.y;
	if (minPos > 0.f)
		minPos = 0.f;
	targetPos = std::max(position, minPos);

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

int CutsceneRenderer::getSegmentIndex() const
{
	return m_segmentIndex;
}

int CutsceneRenderer::getInternalSegmentIndex(int index) const
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

void CutsceneRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
	m_icon.getText().setCharacterSize(size.y * 0.05f);
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

void CutsceneRenderer::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
	reset(true);
}

float CutsceneRenderer::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
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
	auto text = segment->getText();
	auto activeText = ActiveText::fromCutsceneTextSegment(segment);
	auto effect = segment->getTransition();
	auto duration = 0.001f * segment->getFullDuration();

	// Push activeText in callback so it doesn't show before update()
	TweenEngine::Tween::mark()
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this, activeText](TweenEngine::BaseTween*){
			m_texts.push_back(activeText);
		}).start(m_tweenManager);

/*
	if (effect == CutsceneSegment::TextEffectFade) {
		activeText->setAlpha(0.f);
		TweenEngine::Tween::to(*activeText, ActiveText::ALPHA, duration)
			.target(255.f)
			.start(m_tweenManager);
	}
	else if (effect == CutsceneSegment::TextEffectFadeAcross) {
		activeText->setFadeAcrossPosition(0.f);
		TweenEngine::Tween::to(*activeText, ActiveText::FADEACROSS, duration)
			.ease(TweenEngine::TweenEquations::easeInOutLinear)
			.target(1.f)
			.start(m_tweenManager);
	}
*/
}

void CutsceneRenderer::startTransitionEffect(const CutscenePageBreakSegment *segment)
{
	auto effect = segment->getTransition();
	auto duration = 0.001f * segment->getFullDuration();

	for (auto &text : m_textsOld)
	{
		if (effect == CutsceneSegment::PageEffectNone) {
			TweenEngine::Tween::set(*text, ActiveText::ALPHA)
				.target(0.f)
				.delay(duration)
				.start(m_tweenManager);
		}
		else if (effect == CutsceneSegment::PageEffectFade) {
			TweenEngine::Tween::to(*text, ActiveText::ALPHA, duration)
				.target(0.f)
				.start(m_tweenManager);
		}
		else if (effect == CutsceneSegment::PageEffectScrollLeft) {
			TweenEngine::Tween::to(*text, ActiveText::POSITION_X, duration)
				.target(-getPosition().x - text->getSize().x - m_margin)
				.start(m_tweenManager);
		}
	}
}

void CutsceneRenderer::addSegmentToQueue(size_t segmentIndex)
{
	m_scrollBar.setPosition(m_size.x - 4.f, 4.f);
	m_scrollBar.setSize(sf::Vector2f(2, m_size.y - 8.f));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(m_size.x, m_size.y - m_margin*2));

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
			// This gives some padding at the bottom for text
			auto scrollAreaMargin = m_margin * 2;

			auto activeText = ActiveText::fromCutsceneTextSegment(seg);
			activeText->reset();
			activeText->setSize(sf::Vector2f((m_size.x < m_size.y ? 1.f : 0.6f) * m_size.x - m_margin*2, m_size.y));
			activeText->setFontSizeMultiplier(m_fontSizeMultiplier);
			activeText->setSkipWaitingForClick(m_skipWaitingForClick);
			activeText->setSkipWaitingForClick(true); // TODO: set this right
			if (seg->getBeginWithNewLine()) {
				m_cursorPos.x = 0.f;
				m_cursorPos.y = std::max(0.f, m_scrollAreaSize.y - scrollAreaMargin);
			}

			auto startPos = m_cursorPos.y;
			activeText->setCursorStart(m_cursorPos);
			m_cursorPos = activeText->getCursorEnd();
			m_timeToNext = sf::milliseconds(seg->getFullDelay());

			// Push activeText in callback so it doesn't show before update()
			TweenEngine::Tween::mark()
				.setCallback(TweenEngine::TweenCallback::BEGIN, [this, activeText](TweenEngine::BaseTween*){
					m_texts.push_back(activeText);
				}).start(m_tweenManager);

			m_scrollAreaSize.y = activeText->getLocalBounds().height + scrollAreaMargin;
			updateScrollbar();

			if (m_cursorPos.y + 60.f > m_size.y - m_margin*2 - m_scrollPos) {
				setScrollTween(-startPos, 1.f);
			}
			repositionItems();
		};
	}
	else if (type == CutsceneSegment::PageBreak)
	{
		auto seg = static_cast<CutscenePageBreakSegment*>(segment.get());

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
	else if (type == CutsceneSegment::Script)
	{
		auto seg = static_cast<CutsceneScriptSegment*>(segment.get());
		beginCallback = [this, seg](TweenEngine::BaseTween*)
		{
			m_timeToNext = sf::milliseconds(seg->getDelay());
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
	repositionItems();
}

void CutsceneRenderer::waitForClick()
{
	m_isWaitingForClick = true;
	auto posY = std::min(m_scrollAreaSize.y, m_size.y - m_margin * 2);
	m_icon.setPosition(m_size.x / 2, posY);
	m_icon.show(2.f);
}

} // namespace NovelTea
