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
, m_size(400.f, 400.f)
{
	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonTexture = texture.get();
	setDialogue(std::make_shared<Dialogue>());
}

void DialogueRenderer::setDialogue(const std::shared_ptr<Dialogue> &dialogue)
{
	m_dialogue = dialogue;
	reset();
}

void DialogueRenderer::reset()
{
	m_isComplete = false;
	m_tweenManager.killAll();
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
	for (auto &button : m_buttons)
		button.processEvent(event);
	return true;
}

bool DialogueRenderer::processSelection(int buttonIndex)
{
	if (m_isComplete)
		return false;
	if (buttonIndex < 0 || buttonIndex >= m_buttons.size())
		return false;
	m_buttons[buttonIndex].click();
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

	m_buttons.clear();
	m_currentSegmentIndex = newSegmentIndex;
	m_nextForcedSegmentIndex = -1;
	std::shared_ptr<DialogueSegment> npcSegment = nullptr;
	float posY;
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
	posY = m_middleY;
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
		Button btn;
		btn.setString(seg->getText());
		btn.setTexture(m_buttonTexture);
		btn.setPosition(10.f, posY);
		btn.setContentSize(m_size.x-30.f, 25.f);
		btn.setColor(sf::Color::Yellow);
		btn.setActiveColor(sf::Color::Red);
		btn.onClick([this, i, childId](){
			if (m_callback)
				m_callback(i);
			changeSegment(childId);
		});
		btn.getText().setCharacterSize(20);

		++i;
		posY += btn.getSize().y + 2.f;
		m_buttons.emplace_back(std::move(btn));
	}

	changeLine(0);
}

void DialogueRenderer::changeLine(int newLineIndex)
{
	if (newLineIndex + 1 > m_textLines.size())
		return;
	auto &line = m_textLines[newLineIndex];
	m_textLineIndex = newLineIndex;
	m_textOld = m_text;
	m_text.setText(line.second);
	m_text.setPosition(10.f, round(m_middleY - m_text.getCursorEnd().y - 60.f));
	m_textName.setText(line.first);
	m_textName.setPosition(5.f, m_text.getPosition().y - 40.f);

	float duration = 1.5f;
	m_text.setAlpha(0.f);
	m_textOld.setAlpha(255.f);
	TweenEngine::Tween::to(m_textOld, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
}

bool DialogueRenderer::isComplete() const
{
	return m_isComplete;
}

void DialogueRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
	m_middleY = m_size.y / 2.f - 100.f;
}

sf::Vector2f DialogueRenderer::getSize() const
{
	return m_size;
}

void DialogueRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(m_text, states);
	target.draw(m_textOld, states);
	target.draw(m_textName, states);
	for (auto &button : m_buttonsOld)
		target.draw(button, states);
	for (auto &button : m_buttons)
		target.draw(button, states);
}

} // namespace NovelTea
