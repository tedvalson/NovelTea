#include <NovelTea/ProjectData.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

ActionBuilder::ActionBuilder()
: m_visible(false)
, m_isHiding(false)
, m_isShowing(false)
, m_callback(nullptr)
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

ActionBuilder::~ActionBuilder()
{
}

void ActionBuilder::update(float delta)
{
	m_tweenManager.update(delta);
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

void ActionBuilder::show()
{
	if (!m_isShowing)
	{
		m_visible = true;
		m_isShowing = true;
		TweenEngine::Tween::to(*this, ALPHA, 0.4f)
			.target(255.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_isShowing = false;
			}).start(m_tweenManager);
	}
}

void ActionBuilder::hide()
{
	if (!m_isHiding)
	{
		m_isHiding = true;
		TweenEngine::Tween::to(*this, ALPHA, 0.4f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_visible = false;
				m_isHiding = false;
			}).start(m_tweenManager);
	}
}

bool ActionBuilder::isVisible() const
{
	return m_visible;
}

void ActionBuilder::setVerb(const std::string &verbId)
{
	m_verbId = verbId;
	auto verb = Proj.get<Verb>(verbId);
	if (verb)
	{
		m_objectIds.resize(verb->getObjectCount());
		for (auto &objectId : m_objectIds)
			objectId.clear();
	}
}

void ActionBuilder::setObject(const std::string &objectId, size_t index)
{
	if (index < m_objectIds.size())
	{
		m_objectIds[index] = objectId;
		updateText();
	}
}

std::string ActionBuilder::getVerb() const
{
	return m_verbId;
}

std::vector<std::string> ActionBuilder::getObjects() const
{
	return m_objectIds;
}

std::shared_ptr<Action> ActionBuilder::getAction() const
{
	return Proj.findAction(m_verbId, m_objectIds);
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
	auto verb = Proj.get<Verb>(m_verbId);
	if (verb)
	{
		m_text.setText(verb->getActionText(m_objectIds));

	}
}

void ActionBuilder::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_text, states);
	target.draw(m_buttonConfirm, states);
	target.draw(m_buttonCancel, states);
}

void ActionBuilder::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case ALPHA: {
			sf::Color color;
			m_text.setAlpha(newValues[0]);
			SET_ALPHA(m_buttonCancel.getFillColor, m_buttonCancel.setFillColor, 255.f);
			SET_ALPHA(m_buttonConfirm.getFillColor, m_buttonConfirm.setFillColor, 255.f);
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int ActionBuilder::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case ALPHA:
			returnValues[0] = m_buttonCancel.getFillColor().a;
		return 1;
	default:
		return TweenTransformable::getValues(tweenType, returnValues);
	}
}

} // namespace NovelTea
