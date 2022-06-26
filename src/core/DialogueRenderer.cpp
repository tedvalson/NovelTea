#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/ActiveTextSegment.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

namespace NovelTea
{

DialogueRenderer::DialogueRenderer()
: m_callback(nullptr)
, m_isShowing(false)
, m_logCurrentIndex(false)
, m_textLineIndex(-1)
, m_fontSize(22.f)
, m_fontSizeMultiplier(1.f)
, m_fadeTween(nullptr)
{
	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonTexture = texture.get();

	m_bg.setTexture(m_buttonTexture);
	m_bg.setColor(sf::Color(0, 0, 0, 0));

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.attachObject(this);

	m_iconContinue.hide(0.f);

	m_animProps.type = TextEffect::FadeAcross;
	m_animProps.duration = -1;
	m_animProps.delay = -1;
	m_animProps.waitForClick = true;

	m_animNameProps.type = TextEffect::None;
	m_animNameProps.duration = 0;
	m_animNameProps.delay = 0;

	m_size = sf::Vector2f(400.f, 400.f);
	setDialogue(std::make_shared<Dialogue>());
}

void DialogueRenderer::setDialogue(const std::shared_ptr<Dialogue> &dialogue)
{
	m_dialogue = dialogue;
	reset();
}

const std::shared_ptr<Dialogue> &DialogueRenderer::getDialogue() const
{
	return m_dialogue;
}

void DialogueRenderer::reset()
{
	hide(0.f);
	m_tweenManager.update(0.1f);

	m_isComplete = false;
	m_text.setText("");
	m_text.show(0.f);
	m_text.onComplete(nullptr);
	m_textName.setText("");
	m_textName.show(0.f);
	m_nextForcedSegmentIndex = m_dialogue->getRootIndex();
	applyChanges();
}

void DialogueRenderer::update(float delta)
{
	if (m_text.isWaitingForClick() && !m_iconContinue.isShowing())
		m_iconContinue.show();

	m_text.update(delta);
	m_textOld.update(delta);
	m_textName.update(delta);
	m_textNameOld.update(delta);
	for (auto& text : m_buttonTexts)
		text->update(delta);
	for (auto& text : m_buttonTextsOld)
		text->update(delta);

	m_scrollBar.update(delta);
	m_iconContinue.update(delta);
	m_tweenManager.update(delta);
}

bool DialogueRenderer::processEvent(const sf::Event &event)
{
	if (m_isShowing)
		return true;
	if (m_scrollBar.processEvent(event))
		return false;

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (!m_text.isComplete()) {
			m_iconContinue.hide(0.4f);
			m_text.click();
			return true;
		}

		auto sizeY = m_scrollBar.getScrollAreaSize().y;
		if (m_scrollPos > 1.05f * sizeY - m_scrollAreaSize.y) {
			TweenEngine::Tween::to(m_scrollBar, ScrollBar::SCROLLPOS, 0.3f)
				.targetRelative(-0.75f * sizeY)
				.start(m_tweenManager);
			return true;
		}
		if (continueToNext())
			return true;
	}
	// Need to break loop since button click could change m_buttons
	for (auto &button : m_buttons)
		if (button->getAlpha() == 255.f && button->processEvent(event))
			break;
	return true;
}

void DialogueRenderer::processLines()
{
	while (m_buttons.empty() && !isComplete())
		changeSegment(m_nextForcedSegmentIndex);
	if (!m_textLines.empty())
		changeLine(m_textLines.size() - 1);
}

bool DialogueRenderer::processSelection(int buttonIndex)
{
	processLines();
	if (m_isComplete)
		return false;
	if (buttonIndex < 0 || buttonIndex >= m_buttons.size())
		return false;
	m_buttons[buttonIndex]->click();
	return true;
}

void DialogueRenderer::setDialogueCallback(DialogueCallback callback)
{
	m_callback = callback;
}

void DialogueRenderer::repositionButtons(float fontSize)
{
	if (fontSize < 1.f)
		return;
	auto width = (m_size.x < m_size.y ? 0.98f : 0.6f) * m_size.x;
	auto posY = m_bg.getPosition().y + m_bg.getSize().y * 1.1f;
	for (int i = 0; i < m_buttons.size(); ++i)
	{
		auto &button = m_buttons[i];
		auto &text = m_buttonTexts[i];
		auto &padding = button->getPadding();
		TextProperties textProps;
		textProps.fontSize = 0.5f * fontSize;
		textProps.color = button->getTextColor();
		text->setSize(sf::Vector2f(width - padding.left * 2, 0.f));
		text->setLineSpacing(0.1f * fontSize);
		text->updateProps(textProps, AnimationProperties());
		button->setSize(width, text->getLocalBounds().height + (padding.top + padding.height) * fontSize * 0.08f);

		button->setPosition(m_bg.getPosition().x, round(posY));
		text->setPosition(m_bg.getPosition().x + padding.left, round(posY) + padding.top);
		posY += button->getSize().y + fontSize * 0.1f;
	}

	if (posY > m_size.y)
		repositionButtons(0.9f * fontSize);
}

void DialogueRenderer::applyChanges()
{
	auto portrait = m_size.x < m_size.y;
	auto posX = (portrait ? 0.01f : 0.2f) * m_size.x;
	m_padding = (portrait ? 0.01f : 0.004f) * m_size.x;
	m_fontSize = m_fontSizeMultiplier * 22;
	m_text.setSize(sf::Vector2f((portrait ? 0.95f : 0.58f) * m_size.x, m_size.y));

	TextProperties textProps;
	textProps.fontSize = m_fontSize / 2;
	m_textName.updateProps(textProps, m_animNameProps);
	m_text.updateProps(textProps, m_animProps);

	m_textName.setPosition(round(posX + m_padding), round(2.f * m_size.y / m_fontSize));
	m_bg.setPosition(round(posX), m_textName.getPosition().y + 1.2f * m_fontSize);
	m_bg.setSize((portrait ? 0.98f : 0.6f) * m_size.x,
				 std::min(0.3f * m_size.y, m_fontSize * (portrait ? 6 : 5)));

	m_iconContinue.getText().setCharacterSize(m_fontSize);
	m_iconContinue.setPosition(posX + m_bg.getSize().x - m_fontSize, m_bg.getPosition().y + m_bg.getSize().y - m_fontSize * 1.1f);

	m_scrollBar.setPosition(posX + m_bg.getSize().x - 4.f, m_bg.getPosition().y);
	m_scrollBar.setSize(sf::Vector2f(2, m_bg.getSize().y));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(0, m_bg.getSize().y - m_padding*2));
	m_scrollBar.setDragRect(sf::FloatRect(0.f, 0.f, m_size.x, m_bg.getPosition().y + m_bg.getSize().y));

	m_scrollAreaSize.y = m_text.getCursorEnd().y + m_fontSize * 2;
	updateScrollbar();

	auto top = (m_bg.getPosition().y + m_padding) / m_size.y;
	auto left = (m_bg.getPosition().x + m_padding) / m_size.x;
	auto width = m_bg.getSize().x - m_padding*2;
	auto height = m_bg.getSize().y - m_padding*2;
	m_view.reset(sf::FloatRect(0.f, 0.f, width, height));
	m_view.setViewport(sf::FloatRect(left, top, width / m_size.x, height / m_size.y));

	m_scrollBar.setScroll(0.f);
	repositionText();
	repositionButtons(m_fontSize);
}

// Segment arg is a choice segment (or root/link)
void DialogueRenderer::changeSegment(int newSegmentIndex, bool run, int buttonSubindex)
{
	m_bg.setColor(sf::Color(0, 0, 0, 30));

	if (newSegmentIndex < 0) {
		m_isComplete = true;
		return;
	}

	m_dialogue->getPropertyList()->sync();
	m_tweenManager.killAll();
	m_buttonsOld = m_buttons;
	m_buttons.clear();
	m_buttonTextsOld = m_buttonTexts;
	m_buttonTexts.clear();
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
				ActiveGame->getTextLog()->push(text, TextLogType::DialogueOption);
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
	if (childIds.empty() && textSegment->isEmpty() && m_buttons.empty()) {
		m_isComplete = true;
		return;
	}

	for (auto &button : m_buttons) {
		TweenEngine::Tween::set(*button, Button::ALPHA)
			.target(0.f)
			.start(m_tweenManager);
	}
	for (auto &button : m_buttonsOld) {
		TweenEngine::Tween::to(*button, Button::ALPHA, 0.4f)
			.target(0.f)
			.start(m_tweenManager);
	}
	for (auto &text : m_buttonTexts)
		text->hide(0.f);
	for (auto &text : m_buttonTextsOld)
		text->hide(0.4f);
	m_tweenManager.update(0.0001f);

	changeLine(0);
}

void DialogueRenderer::changeLine(int newLineIndex)
{
	if (newLineIndex + 1 > m_textLines.size())
		return;
	auto &line = m_textLines[newLineIndex];
	TextProperties textProps;
	textProps.fontSize = m_fontSize / 2;
	m_textLineIndex = newLineIndex;
	while (!m_text.isComplete())
	{
		m_text.skipToNext();
	}

	m_textOld = m_text;
	m_text.setText(line.second, textProps, m_animProps);

	m_scrollAreaSize.y = m_text.getCursorEnd().y + m_fontSize * 2;
	updateScrollbar();
	m_scrollBar.setScroll(0.f);

	if (m_logCurrentIndex) {
		ActiveGame->getTextLog()->push(line.first, TextLogType::DialogueTextName);
		ActiveGame->getTextLog()->push(line.second, TextLogType::DialogueText);
	}

	// Check last segment to see if it expects to wait for click
	const auto& segs = m_text.getSegments();
	auto noWait = !segs.empty() && !segs[segs.size()-1]->getAnimProps().waitForClick;

	m_text.onComplete([this, newLineIndex, noWait](){
		if (noWait && continueToNext())
			return;

		if (m_textLineIndex + 1 == m_textLines.size()) {
			repositionButtons(m_fontSize);
			if (m_buttons.empty())
				m_iconContinue.show();
			for (auto &button : m_buttons) {
				TweenEngine::Tween::to(*button, Button::ALPHA, 1.f)
					.target(255.f)
					.start(m_tweenManager);
			}
			for (auto &text : m_buttonTexts) {
				text->hide(0.f);
				text->show(1.f);
			}
		} else {
			m_iconContinue.show();
		}
	});

	m_iconContinue.hide(0.4f);

	float duration = 0.3f;
	m_textOld.hide(duration);
	m_text.show(0.f);
	m_textName.show(0.f);
	m_textNameOld = m_textName;
	m_textName.setText(line.first, textProps, m_animNameProps);
	m_textName.hide(0.f);
	m_textName.show(duration);
	m_textNameOld.hide(duration);
}

bool DialogueRenderer::continueToNext()
{
	if (m_textLineIndex + 1 == m_textLines.size()) {
		// Cannot continue if waiting for option to be selected
		if (m_buttons.empty()) {
			changeSegment(m_nextForcedSegmentIndex);
			return true;
		}
	} else {
		changeLine(m_textLineIndex + 1);
		return true;
	}
	return false;
}

sj::JSON DialogueRenderer::saveState() const
{
	auto index = m_nextForcedSegmentIndex;
	if (index < 0)
		index = m_currentSegmentIndex;
	return sj::Array(index);
}

void DialogueRenderer::restoreState(const sj::JSON &jstate)
{
	auto segmentIndex = jstate[0].ToInt();
	if (segmentIndex < 0 || segmentIndex >= m_dialogue->segments().size())
		return;
	reset();
	show(1.f, segmentIndex);
}

bool DialogueRenderer::isComplete() const
{
	return m_isComplete;
}

void DialogueRenderer::show(float duration, int startSegmentIndex)
{
	m_isShowing = true;
	m_tweenManager.killAll();
	if (startSegmentIndex < 0)
		startSegmentIndex = m_dialogue->getRootIndex();
	m_scrollBar.setAutoHide(false);
	m_scrollAreaSize.y = 0.f;
	updateScrollbar();
	m_scrollBar.show();
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(30.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this, startSegmentIndex](TweenEngine::BaseTween*){
			m_isShowing = false;
			changeSegment(startSegmentIndex, false);
		})
		.start(m_tweenManager);
}

void DialogueRenderer::hide(float duration)
{
	m_tweenManager.killAll();
	m_scrollBar.setAutoHide(true); // Prevent showing after hide
	m_scrollBar.hide();
	m_iconContinue.hide(duration);
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	m_textName.hide(duration);
	m_textNameOld.hide(duration);
	m_text.hide(duration);
	m_textOld.hide(duration);
	for (auto &button : m_buttons) {
		TweenEngine::Tween::to(*button, Button::ALPHA, duration)
			.target(0.f)
			.start(m_tweenManager);
	}
	for (auto &button : m_buttonsOld) {
		TweenEngine::Tween::to(*button, Button::ALPHA, duration)
			.target(0.f)
			.start(m_tweenManager);
	}
	for (auto &text : m_buttonTexts)
		text->hide(duration);
	for (auto &text : m_buttonTextsOld)
		text->hide(duration);
}

void DialogueRenderer::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionText();
}

float DialogueRenderer::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &DialogueRenderer::getScrollSize()
{
	return m_scrollAreaSize;
}

void DialogueRenderer::repositionText()
{
	m_text.setPosition(m_padding, m_padding + m_scrollPos);
}

void DialogueRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
	applyChanges();
}

sf::Vector2f DialogueRenderer::getSize() const
{
	return m_size;
}

void DialogueRenderer::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
	applyChanges();
}

float DialogueRenderer::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
}

void DialogueRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

	target.draw(m_bg, states);

	auto view = target.getView();
	target.setView(m_view);
	target.draw(m_text, states);
	target.draw(m_textOld, states);
	target.setView(view);

	target.draw(m_textName, states);
	target.draw(m_textNameOld, states);
	for (auto &button : m_buttonsOld)
		target.draw(*button, states);
	for (auto &button : m_buttons)
		target.draw(*button, states);
	for (auto &text : m_buttonTextsOld)
		target.draw(*text, states);
	for (auto &text : m_buttonTexts)
		target.draw(*text, states);
	target.draw(m_scrollBar);
	target.draw(m_iconContinue);
}

void DialogueRenderer::genOptions(const std::shared_ptr<DialogueSegment> &parentNode, bool isRoot)
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

			auto btn = new Button;
			btn->setCentered(false);
			btn->setTexture(m_buttonTexture);
			// Check if button is enabled
			if (m_dialogue->getEnableDisabledOptions() || !disabled) {
				hasWorkingOption = true;
				btn->setColor(sf::Color(180, 180, 180, 180));
				btn->setActiveColor(sf::Color(140, 140, 140));
				if (disabled)
					btn->setTextColor(sf::Color(100, 100, 100));
				else
					btn->setTextColor(sf::Color::Black);
				btn->onClick([=](){
					if (m_callback)
						m_callback(buttonIndex);
					if (seg->getShowOnce())
						m_dialogue->setSegmentHasShown(seg->getId(), i);
					changeSegment(seg->getId(), true, i);
				});
			} else {
				auto bgColor = sf::Color(180, 180, 180, 100);
				auto textColor = sf::Color(100, 100, 100);
				btn->setColor(bgColor);
				btn->setActiveColor(bgColor);
				btn->setTextColor(textColor);
			}

			++buttonIndex;
			++i;
			m_buttons.emplace_back(btn);

			TextProperties buttonTextProps;
			buttonTextProps.fontSize = m_fontSize / 2;
			m_buttonTexts.emplace_back(new ActiveText(buttonText, buttonTextProps));
		}
	}

	if (isRoot && !hasWorkingOption) {
		m_buttons.clear();
		m_buttonTexts.clear();
	}
}

} // namespace NovelTea
