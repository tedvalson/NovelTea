#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/Navigation.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

Navigation::Navigation()
: m_needsUpdate(true)
, m_alpha(255.f)
, m_callback(nullptr)
{
	m_paths = sj::Array();
	for (int i = 0; i < 8; ++i)
	{
		auto button = new TweenRectangleShape;
		m_buttons.emplace_back(button);
		m_paths.append(sj::Array(false, sj::Object()));
	}

	setSize(sf::Vector2f(150.f, 150.f));
}

Navigation::~Navigation()
{
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
			{
				m_callback(m_paths[i][1]);
				return true;
			}
		}
	}
	return false;
}

void Navigation::setSize(const sf::Vector2f &size)
{
	m_needsUpdate = true;
	m_size = size;
}

sf::Vector2f Navigation::getSize() const
{
	return m_size;
}

void Navigation::setPaths(const json &value)
{
	m_paths = value;

	for (int i = 0; i < 8; ++i)
	{
		if (m_paths[i][0].ToBool() && m_paths[i][1][0].ToInt() != -1)
		{
			m_buttons[i]->setFillColor(sf::Color::Green);
		} else {
			m_buttons[i]->setFillColor(sf::Color::Blue);
		}
	}

	// Reset alpha to reflect new button states
	setValues(ALPHA, &m_alpha);
}

const json &Navigation::getPaths() const
{
	return m_paths;
}

sf::FloatRect Navigation::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect Navigation::getGlobalBounds() const
{
	ensureUpdate();
	return getTransform().transformRect(m_bounds);
}

void Navigation::setCallback(NavigationCallback callback)
{
	m_callback = callback;
}

void Navigation::setAlpha(float alpha)
{
	sf::Color color;
	m_alpha = alpha;
	float *newValues = &m_alpha;
	for (int i = 0; i < m_buttons.size(); ++i)
	{
		float alphaMax = m_paths[i][0].ToBool() ? 255.f : 40.f;
		SET_ALPHA(m_buttons[i]->getFillColor, m_buttons[i]->setFillColor, alphaMax);
	}
}

float Navigation::getAlpha() const
{
	return m_alpha;
}

void Navigation::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	int buttonIndex = 0;
	for (int i = 0; i < 9; ++i)
	{
		int x = i % 3;
		int y = i / 3;
		if (x == 1 && y == 1)
			continue;
		auto &button = m_buttons[buttonIndex];
		button->setSize(sf::Vector2f(32.f, 32.f));
		button->setPosition(36.f * x, 36.f * y);
		++buttonIndex;
	}

	m_bounds.left = getPosition().x;
	m_bounds.top = getPosition().y;
	m_bounds.width = 36.f * 3;
	m_bounds.height = 36.f * 3;

	m_needsUpdate = false;
}

void Navigation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	states.transform *= getTransform();
	for (auto &button : m_buttons)
		target.draw(*button, states);
}

} // namespace NovelTea
