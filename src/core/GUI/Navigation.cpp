#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/Navigation.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

Navigation::Navigation()
: m_alpha(255.f)
, m_visible(false)
, m_isHiding(false)
, m_isShowing(false)
, m_callback(nullptr)
{
	m_paths = sj::Array();
	for (int i = 0; i < 9; ++i)
	{
		int x = i % 3;
		int y = i / 3;
		if (x == 1 && y == 1)
			continue;
		auto button = new TweenRectangleShape;
		button->setFillColor(sf::Color::Blue);
		button->setSize(sf::Vector2f(32.f, 32.f));
		button->setPosition(36.f * x, 36.f * y);
		m_buttons.emplace_back(button);
		m_paths.append(sj::Array(false, sj::Object()));
	}

	TweenEngine::Tween::set(*this, ALPHA)
		.target(0.f)
		.start(m_tweenManager);
}

Navigation::~Navigation()
{
}

void Navigation::update(float delta)
{
	m_tweenManager.update(delta);
}

bool Navigation::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		for (int i = 0; i < m_buttons.size(); ++i)
		{
			if (!m_paths[i][0].ToBool() || m_paths[i][1][0] == -1)
				continue;
			if (m_buttons[i]->getGlobalBounds().contains(p))
				m_callback(m_paths[i][1]);
		}
	}
	return true;
}

void Navigation::show()
{
	if (!m_isShowing)
	{
		m_visible = true;
		m_isShowing = true;
		TweenEngine::Tween::to(*this, ALPHA, 1.f)
			.target(255.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_isShowing = false;
			}).start(m_tweenManager);
	}
}

void Navigation::hide()
{
	if (!m_isHiding)
	{
		m_isHiding = true;
		TweenEngine::Tween::to(*this, ALPHA, 0.5f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_visible = false;
				m_isHiding = false;
			}).start(m_tweenManager);
	}
}

bool Navigation::isVisible() const
{
	return m_visible;
}

void Navigation::setSize(const sf::Vector2f &size)
{
	m_size = size;
}

sf::Vector2f Navigation::getSize() const
{
	return m_size;
}

void Navigation::setPaths(const json &value)
{
	for (int i = 0; i < 8; ++i)
	{
		if (value[i][0].ToBool() && value[i][1][0].ToInt() != -1)
		{
			m_buttons[i]->setFillColor(sf::Color::Green);
		} else {
			m_buttons[i]->setFillColor(sf::Color::Blue);
		}
	}
	m_paths = value;

	// Reset alpha to reflect new button states
	TweenEngine::Tween::set(*this, ALPHA)
		.target(m_alpha)
		.start(m_tweenManager);
}

const json &Navigation::getPaths() const
{
	return m_paths;
}

void Navigation::setCallback(NavigationCallback callback)
{
	m_callback = callback;
}

void Navigation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	for (auto &button : m_buttons)
		target.draw(*button, states);
}

void Navigation::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case ALPHA: {
			sf::Color color;
			m_alpha = newValues[0];
			for (int i = 0; i < m_buttons.size(); ++i)
			{
				float alphaMax = m_paths[i][0].ToBool() ? 255.f : 40.f;
				SET_ALPHA(m_buttons[i]->getFillColor, m_buttons[i]->setFillColor, alphaMax);
			}
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int Navigation::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case ALPHA:
			returnValues[0] = m_alpha;
		return 1;
	default:
		return TweenTransformable::getValues(tweenType, returnValues);
	}
}

} // namespace NovelTea
