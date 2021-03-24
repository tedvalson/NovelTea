#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
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
, m_fadeTween(nullptr)
{
	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonTexture = texture.get();

	m_bg.setTexture(m_buttonTexture);
	m_bg.setColor(sf::Color(0, 0, 0, 0));

	setSize(sf::Vector2f(400.f, 400.f));
	setDialogue(std::make_shared<Dialogue>());
}

void DialogueRenderer::setDialogue(const std::shared_ptr<Dialogue> &dialogue)
{
	m_dialogue = dialogue;
	reset();
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
	m_tweenManager.update(delta);
}

bool DialogueRenderer::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (m_text.getFadeAcrossPosition() < 1.f) {
			if (m_fadeTween)
				m_fadeTween->update(9999.f);
			return true;
		}
		if (m_textLineIndex < m_textLines.size() - 1) {
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

void DialogueRenderer::repositionButtons()
{
	auto width = (m_size.x < m_size.y ? 0.98f : 0.6f) * m_size.x;
	auto posY = m_text.getPosition().y + m_bg.getSize().y + m_fontSize * 0.4f;
	for (int i = 0; i < m_buttons.size(); ++i)
	{
		auto &button = m_buttons[i];
		auto str = m_buttonStrings[i];
		auto &text = button->getText();
		auto &padding = button->getPadding();
		button->setString(str);
		text.setCharacterSize(m_fontSize);

		if (wrapText(text, width))
			str = text.getString().toAnsiString();
		auto lineCount = 1 + std::count(str.begin(), str.end(), '\n');
		button->setSize(width, m_fontSize * lineCount + (padding.top + padding.height) * 1.9f);

		button->setPosition(m_bg.getPosition().x, round(posY));
		posY += button->getSize().y + m_fontSize * 0.1f;
	}
}

void DialogueRenderer::changeSegment(int newSegmentIndex)
{
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
	std::shared_ptr<DialogueSegment> npcSegment = nullptr;
	auto startSegment = m_dialogue->getSegment(m_currentSegmentIndex);
	startSegment->runScript();

	// Get NPC line
	for (auto childId : startSegment->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses())
			continue;
		if (seg->getShowOnce() && m_dialogue->segmentShown(childId))
			continue;

		npcSegment = seg;
		m_textLines = npcSegment->getTextMultiline();
		break;
	}
	if (npcSegment)
		npcSegment->runScript();
	else {
		m_isComplete = true;
		return;
	}

	genOptions(npcSegment, true);

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
			repositionButtons();
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

bool DialogueRenderer::isComplete() const
{
	return m_isComplete;
}

void DialogueRenderer::show(float duration)
{
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(30.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
			changeSegment(m_dialogue->getRootIndex());
		})
		.start(m_tweenManager);
}

void DialogueRenderer::hide(float duration)
{
	m_tweenManager.killAll();
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

void DialogueRenderer::setSize(const sf::Vector2f &size)
{
	auto portrait = size.x < size.y;
	auto posX = (portrait ? 0.01f : 0.2f) * size.x;
	auto padding = (portrait ? 0.01f : 0.004f) * size.x;
	m_fontSize = 0.05f * std::min(size.x, size.y);
	m_size = size;
	m_text.setSize(sf::Vector2f((portrait ? 0.95f : 0.58f) * size.x, size.y));
	m_middleY = round(m_size.y / 8);

	TextFormat format;
	format.size(m_fontSize/2);
	m_textName.setText(m_textName.getText(), format);
	m_text.setText(m_text.getText(), format);

	m_textName.setPosition(round(posX + padding), m_middleY);
	m_text.setPosition(round(posX + padding * 2),
					   round(padding + m_textName.getPosition().y + 1.2f * m_fontSize));
	m_bg.setPosition(round(posX), m_textName.getPosition().y + 1.2f * m_fontSize);
	m_bg.setSize((portrait ? 0.98f : 0.6f) * size.x, m_fontSize * 5);

	repositionButtons();
}

sf::Vector2f DialogueRenderer::getSize() const
{
	return m_size;
}

void DialogueRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(m_bg, states);
	target.draw(m_text, states);
	target.draw(m_textOld, states);
	target.draw(m_textName, states);
	target.draw(m_textNameOld, states);
	for (auto &button : m_buttonsOld)
		target.draw(*button, states);
	for (auto &button : m_buttons)
		target.draw(*button, states);
}

void DialogueRenderer::genOptions(const std::shared_ptr<DialogueSegment> &parentNode, bool isRoot)
{
	// Get player options
	int i = 0;
	for (auto childId : parentNode->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses())
			continue;
		if (seg->getShowOnce() && m_dialogue->getSegmentHasShown(childId))
			continue;
		if (seg->getTextRaw().empty()) {
			if (m_buttons.empty()) {
				m_nextForcedSegmentIndex = childId;
				if (seg->getShowOnce())
					m_dialogue->setSegmentHasShown(childId);
				if (isRoot && parentNode->getTextRaw().empty()) {
					changeSegment(childId);
					return;
				}
				auto childrenIds = seg->getChildrenIds();
				if (!childrenIds.empty()) {
					auto firstChild = m_dialogue->getSegment(childrenIds[0]);
					if (firstChild->getTextRaw().empty()) {
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
		btn->setColor(sf::Color(180, 180, 180));
		btn->setActiveColor(sf::Color(120, 120, 120));
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
