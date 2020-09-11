#include <NovelTea/Game.hpp>
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

	m_buttonCancel.setSize(sf::Vector2f(45.f, 45.f));
	m_buttonCancel.setFillColor(sf::Color::Red);

	m_textFormat.size(16);

	m_textCancel.setFont(*Proj.getFont(1));

	setAlpha(0.f);
}

void ActionBuilder::update(float delta)
{
	m_tweenManager.update(delta);
	Hideable::update(delta);
}

bool ActionBuilder::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		for (int i = 0; i < m_emptyRects.size(); ++i)
		{
			auto &rect = m_emptyRects[i];
			if (rect->getGlobalBounds().contains(p))
			{
				setSelectedIndex(i);
				return true;
			}
		}

		if (m_buttonCancel.getGlobalBounds().contains(p))
		{
			if (m_callback)
				m_callback(false);
		}
	}
	return true;
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
	SET_ALPHA(m_buttonCancel.getFillColor, m_buttonCancel.setFillColor, 255.f);
}

float ActionBuilder::getAlpha() const
{
	return m_buttonCancel.getFillColor().a;
}

void ActionBuilder::setVerb(const std::string &verbId)
{
	m_verbId = verbId;
	auto verb = GSave.get<Verb>(verbId);
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
	m_buttonCancel.setPosition(size.x - 50.f, 0.f);
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
	auto verb = GSave.get<Verb>(m_verbId);
	auto &actionStructure = verb->getActionStructure();

	auto alpha = getAlpha();
	auto size = sf::Vector2f(m_size.x - 55.f, m_size.y);
	auto lastCursorPos = sf::Vector2f();

	m_tweenManager.killAll();
	m_texts.clear();
	m_emptyRects.clear();

	for (int i = 0; i < actionStructure.size(); ++i)
	{
		std::string s = actionStructure[i] + " ";
		if (i > 0)
		{
			auto objectStr = blankStr;
			auto object = GSave.get<Object>(m_objectIds[i-1]);
			auto rect = new TweenRectangleShape;
			ActiveText tmpText;

			if (!object->getId().empty())
			{
				objectStr = object->getName();
				std::transform(objectStr.begin(), objectStr.end(), objectStr.begin(), ::tolower);
			}
			tmpText.setText(objectStr, m_textFormat);
			auto width = tmpText.getCursorEnd().x;
			rect->setFillColor(m_emptyRectColor);
			rect->setSize(sf::Vector2f(width, 35.f));

			tmpText.setCursorStart(lastCursorPos);
			rect->setPosition(tmpText.getCursorEnd().x - width, tmpText.getCursorEnd().y + 4.f);
			m_emptyRects.emplace_back(rect);
			if (objectStr != blankStr)
				s = objectStr + " " + s;
			else {
				lastCursorPos = tmpText.getCursorEnd();
				s = " " + s;
			}
		}

		auto text = new ActiveText;
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
	target.draw(m_textCancel, states);
}

} // namespace NovelTea
