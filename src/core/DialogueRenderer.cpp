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
	changeSegment(m_dialogue->getRootIndex());
}

void DialogueRenderer::update(float delta)
{
	m_tweenManager.update(delta);
}

bool DialogueRenderer::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
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
		if (button->getAlpha() == 255.f && !button->processEvent(event))
			break;
	return true;
}

bool DialogueRenderer::processSelection(int buttonIndex)
{
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

void DialogueRenderer::changeSegment(int newSegmentIndex)
{
	if (newSegmentIndex < 0) {
		m_isComplete = true;
		return;
	}

	m_tweenManager.killAll();
	m_buttonsOld = m_buttons;
	m_buttons.clear();
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

	// Get player options
	int i = 0;
	for (auto childId : npcSegment->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses())
			continue;
		if (seg->getTextRaw().empty()) {
			if (m_buttons.empty()) {
				m_nextForcedSegmentIndex = childId;
				if (npcSegment->getTextRaw().empty())
					changeSegment(childId);
				return;
			} else
				continue;
		}
		auto btn = new Button;
		auto str = seg->getText();
		auto &text = btn->getText();
		btn->setString(str);
		btn->setCentered(false);
		btn->setTexture(m_buttonTexture);
		btn->setColor(sf::Color(180, 180, 180));
		btn->setActiveColor(sf::Color::Red);
		btn->onClick([this, i, childId](){
			if (m_callback)
				m_callback(i);
			changeSegment(childId);
		});
		text.setCharacterSize(20);

		if (wrapText(text, m_size.x-30.f))
			str = text.getString().toAnsiString();
		auto lineCount = 1 + std::count(str.begin(), str.end(), '\n');
		btn->setContentSize(m_size.x-30.f, 25.f * lineCount);

		++i;
		m_buttons.emplace_back(btn);
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
	m_textLineIndex = newLineIndex;
	m_textOld = m_text;
	m_textNameOld = m_textName;
	m_textName.setText(line.first);
	m_text.setText(line.second);

	float duration = 0.5f;
	m_text.setAlpha(0.f);
	m_textOld.setAlpha(255.f);
	TweenEngine::Tween::to(m_textOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);

	m_textName.setAlpha(0.f);
	m_textNameOld.setAlpha(255.f);
	TweenEngine::Tween::to(m_textNameOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textName, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);

	auto posY = m_text.getPosition().y + m_bg.getSize().y + 10.f;
	if (newLineIndex + 1 == m_textLines.size())
		for (auto &button : m_buttons) {
			button->setPosition(10.f, posY);
			posY += button->getSize().y + 2.f;
			TweenEngine::Tween::to(*button, Button::ALPHA, 1.f)
				.target(255.f)
				.delay(1.f)
				.start(m_tweenManager);
		}
}

bool DialogueRenderer::isComplete() const
{
	return m_isComplete;
}

void DialogueRenderer::show(float duration)
{
	TweenEngine::Tween::to(m_bg, TweenNinePatch::COLOR_ALPHA, duration)
		.target(30.f)
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
	m_size = size;
	m_text.setSize(size);
	m_middleY = round(m_size.y / 8);

	m_textName.setPosition(5.f, m_middleY);
	m_text.setPosition(10.f, m_textName.getPosition().y + 32.f);
	m_bg.setPosition(5.f, m_textName.getPosition().y + 28.f);
	m_bg.setSize(size.x - 10.f, 160.f);
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

} // namespace NovelTea
