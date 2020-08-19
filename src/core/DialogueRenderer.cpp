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
	for (auto &button : m_buttons)
		button.processEvent(event);
	return true;
}

bool DialogueRenderer::processSelection(int buttonIndex)
{
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
	m_buttons.clear();
	m_currentSegmentIndex = newSegmentIndex;
	std::shared_ptr<DialogueSegment> npcSegment = nullptr;
	float posY;
	auto middleY = m_size.y / 2.f - 100.f;
	auto startSegment = m_dialogue->getSegment(m_currentSegmentIndex);
	startSegment->runScript();

	// Get NPC line
	for (auto childId : startSegment->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses(m_dialogue->getId()))
			continue;

		npcSegment = seg;
		m_text.setText(npcSegment->getText(nullptr, m_dialogue->getId()));
		m_text.setPosition(10.f, middleY - m_text.getCursorEnd().y - 60.f);
		break;
	}
	if (npcSegment)
		npcSegment->runScript();
	else {
		m_isComplete = true;
		return;
	}

	// Get player options
	posY = middleY;
	int i = 0;
	for (auto childId : npcSegment->getChildrenIds())
	{
		auto seg = m_dialogue->getSegment(childId);
		if (!seg->conditionPasses(m_dialogue->getId()))
			continue;
		Button btn;
		btn.setString(seg->getText(nullptr, m_dialogue->getId()));
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
}

bool DialogueRenderer::isComplete() const
{
	return m_isComplete;
}

void DialogueRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
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
	for (auto &button : m_buttons)
		target.draw(button, states);
}

} // namespace NovelTea
