#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

ActionBuilder::ActionBuilder()
: m_callback(nullptr)
{
	m_buttonConfirm.setSize(sf::Vector2f(65.f, 65.f));
	m_buttonConfirm.setFillColor(sf::Color::Green);
	m_buttonCancel.setSize(sf::Vector2f(65.f, 65.f));
	m_buttonCancel.setFillColor(sf::Color::Red);
	m_text.setText("test");

	TweenEngine::Tween::set(*this, ALPHA)
		.target(0.f)
		.start(m_tweenManager);
}

bool ActionBuilder::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		if (m_buttonConfirm.getGlobalBounds().contains(p))
		{
			if (m_callback)
				m_callback(true);
		}
		else if (m_buttonCancel.getGlobalBounds().contains(p))
		{
			if (m_callback)
				m_callback(false);
		}
	}
	return true;
}

void ActionBuilder::show(float duration, int tweenType, HideableCallback callback)
{
	TweenEngine::Tween::from(*this, POSITION_Y, duration)
		.target(getPosition().y + 30.f)
		.start(m_tweenManager);
	Hideable::show(duration, tweenType, callback);
}

void ActionBuilder::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, callback);
}

void ActionBuilder::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	m_text.setAlpha(alpha);
	SET_ALPHA(m_buttonCancel.getFillColor, m_buttonCancel.setFillColor, 255.f);
	SET_ALPHA(m_buttonConfirm.getFillColor, m_buttonConfirm.setFillColor, 255.f);
}

float ActionBuilder::getAlpha() const
{
	return m_buttonCancel.getFillColor().a;
}

void ActionBuilder::setVerb(const std::string &verbId)
{
	m_verbId = verbId;
	auto verb = GSave.get<Verb>(verbId);
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
	}
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
	m_text.setSize(sf::Vector2f(size.x - 75.f, size.y));
	m_buttonConfirm.setPosition(size.x - 70.f, 0.f);
	m_buttonCancel.setPosition(size.x - 70.f, 70.f);
}

sf::Vector2f ActionBuilder::getSize() const
{
	return m_size;
}

void ActionBuilder::setCallback(ActionBuilderCallback callback)
{
	m_callback = callback;
}

void ActionBuilder::updateText()
{
	auto verb = GSave.get<Verb>(m_verbId);
	m_text.setText(verb->getActionText(m_objectIds));
}

void ActionBuilder::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_text, states);
	target.draw(m_buttonConfirm, states);
	target.draw(m_buttonCancel, states);
}

} // namespace NovelTea
