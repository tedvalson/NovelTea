#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
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
, m_textLineIndex(-1)
, m_fontSizeMultiplier(1.0)
, m_fadeTween(nullptr)
{
	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonTexture = texture.get();

	m_bg.setTexture(m_buttonTexture);
	m_bg.setColor(sf::Color(0, 0, 0, 0));

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.attachObject(this);

	setSize(sf::Vector2f(400.f, 400.f));
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
	m_textName.setText("");
	m_nextForcedSegmentIndex = m_dialogue->getRootIndex();
}

void DialogueRenderer::update(float delta)
{
	m_scrollBar.update(delta);
	m_tweenManager.update(delta);
}

bool DialogueRenderer::processEvent(const sf::Event &event)
{
	if (m_scrollBar.processEvent(event))
		return false;

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (m_text.getFadeAcrossPosition() < 1.f) {
			if (m_fadeTween)
				m_fadeTween->update(9999.f);
			return true;
		}

		auto sizeY = m_scrollBar.getScrollAreaSize().y;
		if (m_scrollPos > 1.05f * sizeY - m_scrollAreaSize.y) {
			TweenEngine::Tween::to(m_scrollBar, ScrollBar::SCROLLPOS, 0.3f)
				.targetRelative(-0.75f * sizeY)
				.start(m_tweenManager);
			return true;
		} else if (m_textLineIndex < m_textLines.size() - 1) {
			changeLine(m_textLineIndex + 1);
			return true;
		}

		if (m_buttons.empty()) {
			changeSegment(m_nextForcedSegmentIndex);
			return true;
		}
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
		auto str = m_buttonStrings[i];
		auto &text = button->getText();
		auto &padding = button->getPadding();
		auto lineSpacing = text.getFont()->getLineSpacing(fontSize);
		button->setString(str);
		text.setCharacterSize(fontSize);

		if (wrapText(text, width))
			str = text.getString().toAnsiString();
		auto lineCount = 1 + std::count(str.begin(), str.end(), '\n');
		button->setSize(width, lineSpacing * lineCount + (padding.top + padding.height) * 1.9f);

		button->setPosition(m_bg.getPosition().x, round(posY));
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

	TextFormat format;
	format.size(m_fontSize/2);
	m_textName.setText(m_textName.getText(), format);
	m_text.setText(m_text.getText(), format);

	m_textName.setPosition(round(posX + m_padding), round(2.f * m_size.y / m_fontSize));
	m_bg.setPosition(round(posX), m_textName.getPosition().y + 1.2f * m_fontSize);
	m_bg.setSize((portrait ? 0.98f : 0.6f) * m_size.x,
				 std::min(0.3f * m_size.y, m_fontSize * (portrait ? 6 : 5)));

	m_scrollBar.setPosition(posX + m_bg.getSize().x - 4.f, m_bg.getPosition().y);
	m_scrollBar.setSize(sf::Vector2u(2, m_bg.getSize().y));
	m_scrollBar.setScrollAreaSize(sf::Vector2u(0, m_bg.getSize().y - m_padding*2));
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
	repositionButtons(m_fontSize);
}

// Segment arg is a choice segment (or root/link)
void DialogueRenderer::changeSegment(int newSegmentIndex, bool run)
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
	m_buttonStrings.clear();
	m_currentSegmentIndex = newSegmentIndex;
	m_nextForcedSegmentIndex = -1;
	std::shared_ptr<DialogueSegment> textSegment = nullptr;
	auto startSegment = m_dialogue->getSegment(m_currentSegmentIndex);
	auto segText = startSegment->getText();
	if (!segText.empty())
		ActiveGame->getTextLog()->push(segText, TextLogType::DialogueOption);
	if (run)
		startSegment->run();

	// Get text line
	for (auto childId : startSegment->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses())
			continue;
		if (seg->getShowOnce() && m_dialogue->segmentShown(childId))
			continue;

		textSegment = seg;
		m_textLines = textSegment->getTextMultiline();
		break;
	}
	if (textSegment)
		textSegment->run();
	else {
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
	m_tweenManager.update(0.0001f);

	changeLine(0);
}

void DialogueRenderer::changeLine(int newLineIndex)
{
	if (newLineIndex + 1 > m_textLines.size())
		return;
	auto &line = m_textLines[newLineIndex];
	TextFormat format;
	format.size(m_fontSize/2);
	m_textLineIndex = newLineIndex;
	m_text.setFadeAcrossPosition(1.f);
	m_textOld = m_text;
	m_textNameOld = m_textName;
	m_textName.setText(line.first, format);
	m_text.setText(line.second, format);

	m_scrollAreaSize.y = m_text.getCursorEnd().y + m_fontSize * 2;
	updateScrollbar();
	m_scrollBar.setScroll(0.f);

	ActiveGame->getTextLog()->push(line.first, TextLogType::DialogueTextName);
	ActiveGame->getTextLog()->push(line.second, TextLogType::DialogueText);

	float duration = 0.3f;
	m_text.setFadeAcrossPosition(0.f);
	m_text.setAlpha(255.f);
	m_textOld.setAlpha(255.f);
	TweenEngine::Tween::to(m_textOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	m_fadeTween = &TweenEngine::Tween::to(m_text, ActiveText::FADEACROSS, m_text.getFadeAcrossLength() / 220.f)
		.ease(TweenEngine::TweenEquations::easeInOutLinear)
		.target(1.f);
	m_fadeTween->setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*)
	{
		if (m_textLineIndex + 1 == m_textLines.size()) {
			repositionButtons(m_fontSize);
			for (auto &button : m_buttons) {
				TweenEngine::Tween::to(*button, Button::ALPHA, 1.f)
					.target(255.f)
					.start(m_tweenManager);
			}
		}
	}).start(m_tweenManager);

	m_textName.setAlpha(0.f);
	m_textNameOld.setAlpha(255.f);
	TweenEngine::Tween::to(m_textNameOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textName, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
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
	if (startSegmentIndex < 0)
		startSegmentIndex = m_dialogue->getRootIndex();
	m_scrollBar.setAutoHide(false);
	m_scrollAreaSize.y = 0.f;
	updateScrollbar();
	m_scrollBar.show();
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(30.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this, startSegmentIndex](TweenEngine::BaseTween*){
			changeSegment(startSegmentIndex, false);
		})
		.start(m_tweenManager);
}

void DialogueRenderer::hide(float duration)
{
	m_tweenManager.killAll();
	m_scrollBar.setAutoHide(true); // Prevent showing after hide
	m_scrollBar.hide();
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textName, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textNameOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
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
	target.draw(m_scrollBar);
}

void DialogueRenderer::genOptions(const std::shared_ptr<DialogueSegment> &parentNode, bool isRoot)
{
	auto optionNext = parentNode->isOptionNext();

	// Get player options
	int i = 0;
	for (auto childId : parentNode->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (seg->getShowOnce() && m_dialogue->getSegmentHasShown(childId))
			continue;
		if (!seg->conditionPasses())
			continue;
		if (seg->isEmpty()) {
			// If first valid option is empty, pass through
			if (m_buttons.empty()) {
				if (isRoot)
					m_nextForcedSegmentIndex = childId;
				if (seg->getShowOnce())
					m_dialogue->setSegmentHasShown(childId);
				if (isRoot && parentNode->isEmpty()) {
					changeSegment(childId);
					return;
				}
				if (optionNext) {
					// If first child [text] seg is empty, then recurse.
					// Children should exist if optionNext while seg is empty.
					auto firstChild = m_dialogue->getSegment(seg->getChildrenIds()[0]);
					if (firstChild->isEmpty()) {
						seg->run();
						firstChild->run();
						genOptions(firstChild, false);
						return;
					}
				}
				break;
			} else
				continue;
		}
		auto btn = new Button;
		btn->setCentered(false);
		btn->setTexture(m_buttonTexture);
		btn->setColor(sf::Color(180, 180, 180, 180));
		btn->setActiveColor(sf::Color(140, 140, 140));
		btn->setTextColor(sf::Color::Black);
		btn->onClick([this, seg, i, childId](){
			if (m_callback)
				m_callback(i);
			if (seg->getShowOnce())
				m_dialogue->setSegmentHasShown(childId);
			changeSegment(childId);
		});

		++i;
		m_buttons.emplace_back(btn);
		m_buttonStrings.emplace_back(seg->getText());
	}
}

} // namespace NovelTea
