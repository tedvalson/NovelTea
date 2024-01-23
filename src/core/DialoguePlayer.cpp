#include <NovelTea/DialoguePlayer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/Event.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/PropertyList.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

DialoguePlayer::DialoguePlayer(Context *context)
: ContextObject(context)
, m_callback(nullptr)
, m_isShowing(false)
, m_logCurrentIndex(false)
, m_textLineIndex(-1)
{
	m_animProps.type = TextEffect::FadeAcross;
	m_animProps.equation = &TweenEngine::TweenEquations::easeInOutLinear;
	m_animProps.duration = -1;
	m_animProps.delay = -1;
	m_animProps.waitForClick = true;

	m_animNameProps.type = TextEffect::None;
	m_animNameProps.duration = 0;
	m_animNameProps.delay = 0;

	setDialogue(std::make_shared<Dialogue>(context));
}

void DialoguePlayer::setDialogue(const std::shared_ptr<Dialogue> &dialogue)
{
	m_dialogue = dialogue;
	reset();
}

const std::shared_ptr<Dialogue> &DialoguePlayer::getDialogue() const
{
	return m_dialogue;
}

void DialoguePlayer::reset()
{
	m_isComplete = false;
	m_nextForcedSegmentIndex = m_dialogue->getRootIndex();
}

void DialoguePlayer::update(float delta)
{
}

void DialoguePlayer::processLines()
{
	while (m_options.empty() && !isComplete())
		changeSegment(m_nextForcedSegmentIndex);
	if (!m_textLines.empty())
		changeLine(m_textLines.size() - 1);
}

bool DialoguePlayer::processSelection(int buttonIndex)
{
//	processLines();
	if (m_isComplete)
		return false;
	if (buttonIndex < 0 || buttonIndex >= m_options.size())
		return false;
	if (!m_options[buttonIndex]->enabled)
		return false;
	m_options[buttonIndex]->exec();
	return true;
}

void DialoguePlayer::setDialogueCallback(DialogueCallback callback)
{
	m_callback = callback;
}

// Segment arg is a choice segment (or root/link)
void DialoguePlayer::changeSegment(int newSegmentIndex, bool run, int buttonSubindex)
{
	if (newSegmentIndex < 0) {
		m_isComplete = true;
		return;
	}

	m_dialogue->getPropertyList()->sync();
//	m_tweenManager.killAll();
	m_options.clear();
	m_currentSegmentIndex = newSegmentIndex;
	m_nextForcedSegmentIndex = -1;
	m_isShowing = false;

	std::shared_ptr<DialogueSegment> textSegment = nullptr;
	auto startSegment = m_dialogue->getSegment(m_currentSegmentIndex);

	if (startSegment->getType() == DialogueSegment::Type::Text) {
		textSegment = startSegment;
	} else {
		// Run (save) before logging to avoid double-logging on load
		if (run)
			startSegment->run(buttonSubindex);
		if (!startSegment->isEmpty()) {
			auto lines = startSegment->getOptionMultiline();
			auto text = (lines.size() == 1) ? lines[0] : lines[buttonSubindex];
			if (run && startSegment->getIsLogged())
				GTextLog->push(text, TextLogType::DialogueOption);
		}
	}

	// Get text line
	if (!textSegment)
		for (auto childId : startSegment->getChildrenIds())
		{
			auto seg = m_dialogue->getSegment(childId);
			if (!seg->conditionPasses())
				continue;
			if (seg->getShowOnce() && m_dialogue->segmentShown(seg->getId()))
				continue;

			textSegment = seg;
			break;
		}
	if (textSegment) {
		m_textLines = textSegment->getTextMultiline(nullptr, buttonSubindex);
		textSegment->run(buttonSubindex);
		m_logCurrentIndex = textSegment->getIsLogged();
	} else {
		m_isComplete = true;
		return;
	}

	genOptions(textSegment, true);
	// Return if genOptions() changed the segment
	if (m_currentSegmentIndex != newSegmentIndex)
		return;

	// Exit properly when ended on empty text segment
	auto &childIds = textSegment->getChildrenIds();
	if (childIds.empty() && textSegment->isEmpty() && m_options.empty()) {
		m_isComplete = true;
		return;
	}

	changeLine(0);
}

void DialoguePlayer::changeLine(int newLineIndex)
{
	if (newLineIndex + 1 > m_textLines.size())
		return;
	auto &line = m_textLines[newLineIndex];
	TextProperties textProps;
//	textProps.fontSize = m_fontSize / 2;
	m_textLineIndex = newLineIndex;
//	while (!m_text.isComplete())
//	{
//		m_text.skipToNext();
//	}

//	m_textOld = m_text;
//	m_text.setText(line.second, textProps, m_animProps);

	if (m_logCurrentIndex) {
		GTextLog->push(line.first, TextLogType::DialogueTextName);
		GTextLog->push(line.second, TextLogType::DialogueText);
	}

	auto data = StateEvent::DialogueEvent {line.first, line.second, true};
	if (!m_options.empty())
		for (auto& option : m_options)
			data.options.push_back({option->text, (bool)option->exec, option->enabled});
	EventMan->push(data);

	// Check last segment to see if it expects to wait for click
//	const auto& segs = m_text.getSegments();
//	auto noWait = !segs.empty() && !segs.back()->getAnimProps().waitForClick;
}

bool DialoguePlayer::continueToNext()
{
	if (m_textLineIndex + 1 == m_textLines.size()) {
		// Cannot continue if waiting for option to be selected
		if (m_options.empty()) {
			changeSegment(m_nextForcedSegmentIndex);
			return true;
		}
	} else {
		changeLine(m_textLineIndex + 1);
		return true;
	}
	return false;
}

sj::JSON DialoguePlayer::saveState() const
{
	auto index = m_nextForcedSegmentIndex;
	if (index < 0)
		index = m_currentSegmentIndex;
	return sj::Array(index);
}

void DialoguePlayer::restoreState(const sj::JSON &jstate)
{
	auto segmentIndex = jstate[0].ToInt();
	if (segmentIndex < 0 || segmentIndex >= m_dialogue->segments().size())
		return;
	reset();

//	if (segmentIndex < 0)
//		segmentIndex = m_dialogue->getRootIndex();
	changeSegment(segmentIndex, false);
}

bool DialoguePlayer::isComplete() const
{
	return m_isComplete;
}

void DialoguePlayer::genOptions(const std::shared_ptr<DialogueSegment> &parentNode, bool isRoot)
{
	auto optionNext = parentNode->isOptionNext();
	auto hasWorkingOption = false;

	// Get player options
	int buttonIndex = 0;
	for (auto childId : parentNode->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		auto disabled = seg->isDisabled();
		if (!m_dialogue->getShowDisabledOptions()) {
			if (disabled)
				continue;
		}
		if (seg->isEmpty()) {
			// If the one and only option is empty, pass through
			if (parentNode->getChildrenIds().size() == 1) {
				if (isRoot)
					m_nextForcedSegmentIndex = seg->getId();
				if (seg->getShowOnce())
					m_dialogue->setSegmentHasShown(seg->getId());
				if (isRoot && parentNode->isEmpty()) {
					changeSegment(seg->getId());
					return;
				}
				if (optionNext) {
					// If you can find an enabled empty text, recurse.
					// Children should exist if optionNext while seg is empty.
					for (auto id : seg->getChildrenIds()) {
						auto textSeg = m_dialogue->getSegment(id);
						if (textSeg->isEmpty() && !textSeg->isDisabled()) {
							seg->run();
							textSeg->run();
							genOptions(textSeg, false);
							return;
						}
					}
				}
				break;
			} else
				continue;
		}

		int i = 0;
		for (auto &buttonText : seg->getOptionMultiline())
		{
			disabled = seg->isDisabled(i);
			if (!m_dialogue->getShowDisabledOptions()) {
				if (disabled) {
					++i;
					continue;
				}
			}

			auto option = new DialogueOption{buttonText, !disabled, nullptr};
			// Check if button is enabled
			if (m_dialogue->getEnableDisabledOptions() || !disabled) {
				hasWorkingOption = true;
				option->exec = [=](){
					if (m_callback)
						m_callback(buttonIndex);
					if (seg->getShowOnce())
						m_dialogue->setSegmentHasShown(seg->getId(), i);
					changeSegment(seg->getId(), true, i);
				};
			}

			++buttonIndex;
			++i;
			m_options.emplace_back(option);
		}
	}

	if (isRoot && !hasWorkingOption) {
		m_options.clear();
	}
}

} // namespace NovelTea
