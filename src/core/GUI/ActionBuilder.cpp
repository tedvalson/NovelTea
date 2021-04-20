#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <NovelTea/ActiveText.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

ActionBuilder::ActionBuilder()
: m_selectedIndex(0)
, m_callback(nullptr)
{
	m_emptyRectAlpha = 40.f;
	m_emptyRectColor = sf::Color(0.f, 0.f, 200.f, m_emptyRectAlpha);

	m_buttonCancel.getText().setFont(*Proj.getFont(1));
	m_buttonCancel.setString(L"\uf00d");
	m_buttonCancel.setTextColor(sf::Color(255, 0, 0, 200));
	m_buttonCancel.setTextActiveColor(sf::Color(255, 0, 0, 240));
	m_buttonCancel.setColor(sf::Color(0, 0, 0, 30));
	m_buttonCancel.setActiveColor(sf::Color(0, 0, 0, 50));
	m_buttonCancel.onClick([this](){
		if (m_callback)
			m_callback(false);
	});

	setAlpha(0.f);
}

bool ActionBuilder::update(float delta)
{
	m_tweenManager.update(delta);
	Hideable::update(delta);
	return true;
}

// Returns true if interacts with the action builder
bool ActionBuilder::processEvent(const sf::Event &event)
{
	if (!isVisible())
		return false;
	if (m_buttonCancel.processEvent(event))
		return true;

	if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		if (event.type == sf::Event::MouseButtonPressed)
			for (int i = 0; i < m_emptyRects.size(); ++i)
			{
				auto &rect = m_emptyRects[i];
				if (rect->getGlobalBounds().contains(p))
				{
					setSelectedIndex(i);
					return true;
				}
			}

		if (p.x > 0 && p.x < m_size.x && p.y > 0 && p.y < m_size.y)
			return true;
	}

	return false;
}

void ActionBuilder::show(float duration, int tweenType, HideableCallback callback)
{
	if (m_isShowing || isVisible())
		return;

	Hideable::show(duration, tweenType, callback);
	TweenEngine::Tween::from(*this, POSITION_Y, duration)
		.target(getPosition().y + 30.f)
		.start(m_tweenManager);
}

void ActionBuilder::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, callback);
}

void ActionBuilder::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	for (auto &text : m_texts)
		text->setAlpha(alpha);
	for (auto &rect : m_emptyRects) {
		SET_ALPHA(rect->getFillColor, rect->setFillColor, m_emptyRectAlpha);
	}
	m_buttonCancel.setAlpha(alpha);
}

float ActionBuilder::getAlpha() const
{
	return m_buttonCancel.getAlpha();
}

void ActionBuilder::setVerb(const std::string &verbId)
{
	m_verbId = verbId;
	auto verb = GSave->get<Verb>(verbId);
	m_selectedIndex = 0;
	m_objectIds.resize(verb->getObjectCount());
	for (auto &objectId : m_objectIds)
		objectId.clear();
}

void ActionBuilder::setObject(const std::string &objectId, size_t index)
{
	if (index < m_objectIds.size())
	{
		m_objectIds[index] = objectId;
		updateText();
		// Auto-confirm verbs after all objects are filled
		if (m_callback) {
			for (auto &objectId : m_objectIds)
				if (objectId.empty())
					return;
			m_callback(true);
		}
	}
}

void ActionBuilder::setObject(const std::string &objectId)
{
	setObject(objectId, m_selectedIndex);
}

const std::string &ActionBuilder::getVerb() const
{
	return m_verbId;
}

const std::vector<std::string> &ActionBuilder::getObjects() const
{
	return m_objectIds;
}

std::shared_ptr<Action> ActionBuilder::getAction() const
{
	return Action::find(m_verbId, m_objectIds);
}

void ActionBuilder::setSize(const sf::Vector2f &size)
{
	m_size = size;

	m_buttonWidth = 0.08f * size.x;
	m_buttonCancel.setPosition(size.x - m_buttonWidth * 1.2f, 0.f);
	m_buttonCancel.setSize(m_buttonWidth, m_buttonWidth);
	m_buttonCancel.getText().setCharacterSize(m_buttonWidth * 0.8f);
	m_textFormat.size(0.03f * size.x);
	updateText();
}

sf::Vector2f ActionBuilder::getSize() const
{
	return m_size;
}

void ActionBuilder::setSelectedIndex(size_t selectedIndex)
{
	if (selectedIndex < m_emptyRects.size())
	{
		auto &rect = m_emptyRects[selectedIndex];
		m_tweenManager.killAll();
		m_emptyRects[m_selectedIndex]->setFillColor(m_emptyRectColor);
		rect->setFillColor(m_emptyRectColor);
		TweenEngine::Tween::to(*rect, TweenRectangleShape::FILL_COLOR_ALPHA, 1.f)
			.target(150.f)
			.repeatYoyo(-1, 0.f)
			.start(m_tweenManager);
	}
	m_selectedIndex = selectedIndex;
}

size_t ActionBuilder::getSelectedIndex() const
{
	return m_selectedIndex;
}

void ActionBuilder::selectNextEmptyIndex()
{
	for (size_t i = 0; i < m_objectIds.size(); ++i)
		if (m_objectIds[i].empty()) {
			setSelectedIndex(i);
			return;
		}
}

void ActionBuilder::setCallback(ActionBuilderCallback callback)
{
	m_callback = callback;
}

void ActionBuilder::updateText()
{
	std::string blankStr = "_____";
	auto verb = GSave->get<Verb>(m_verbId);
	auto &actionStructure = verb->getActionStructure();

	auto alpha = getAlpha();
	auto size = sf::Vector2f(m_size.x, m_size.y);
	auto lastCursorPos = sf::Vector2f();
	auto offsetX = m_buttonWidth * 0.2f;
	auto offsetY = m_buttonWidth * 1.1f;

	m_tweenManager.killAll();
	m_texts.clear();
	m_emptyRects.clear();

	for (int i = 0; i < actionStructure.size(); ++i)
	{
		std::string s = actionStructure[i] + " ";
		if (i > 0)
		{
			auto objectStr = blankStr;
			auto object = GSave->get<Object>(m_objectIds[i-1]);
			auto rect = new TweenRectangleShape;
			ActiveText tmpText;

			if (!object->getId().empty())
			{
				objectStr = object->getName();
				if (!object->getCaseSensitive())
					std::transform(objectStr.begin(), objectStr.end(), objectStr.begin(), ::tolower);
			}
			tmpText.setSize(size);
			tmpText.setText(objectStr, m_textFormat);
			auto width = tmpText.getCursorEnd().x;
			rect->setFillColor(m_emptyRectColor);
			rect->setSize(sf::Vector2f(width, m_textFormat.size()*2));

			tmpText.setCursorStart(lastCursorPos);
			if (lastCursorPos.y != tmpText.getCursorEnd().y) {
				lastCursorPos = sf::Vector2f(0.f, tmpText.getCursorEnd().y);
				tmpText.setCursorStart(lastCursorPos);
			}
			rect->setPosition(offsetX + tmpText.getCursorEnd().x - width, offsetY + tmpText.getCursorEnd().y + 4.f);
			m_emptyRects.emplace_back(rect);
			if (objectStr != blankStr)
				s = objectStr + " " + s;
			else {
				lastCursorPos = tmpText.getCursorEnd();
				s = " " + s;
			}
		}

		auto text = new ActiveText;
		text->setPosition(offsetX, offsetY);
		text->setSize(size);
		text->setAlpha(alpha);
		text->setCursorStart(lastCursorPos);
		text->setText(s, m_textFormat);
		lastCursorPos = text->getCursorEnd();
		m_texts.emplace_back(text);
	}

	setSelectedIndex(getSelectedIndex());
}

void ActionBuilder::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	for (auto &rect : m_emptyRects)
		target.draw(*rect, states);
	for (auto &text : m_texts)
		target.draw(*text, states);
	target.draw(m_buttonCancel, states);
}

} // namespace NovelTea
