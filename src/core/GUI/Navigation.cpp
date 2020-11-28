#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/Navigation.hpp>
#include <NovelTea/GUI/Button.hpp>
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

	auto font = Proj.getFont(1);
	for (int i = 0; i < 8; ++i)
	{
		auto button = new Button;
		button->getText().setFont(*font);
		button->setString(L"\uf062");
		button->setColor(sf::Color::Transparent);
		button->onClick([this, i](){
			if (!m_paths[i][0].ToBool() || m_paths[i][1][0].ToInt() == -1)
				return;
			m_callback(i, m_paths[i][1]);
		});

		m_buttons.emplace_back(button);
		m_paths.append(sj::Array(false, sj::Object()));
	}

	m_buttons[0]->getText().setRotation(-45.f);
	m_buttons[2]->getText().setRotation(45.f);
	m_buttons[3]->getText().setRotation(-90.f);
	m_buttons[4]->getText().setRotation(90.f);
	m_buttons[5]->getText().setRotation(-135.f);
	m_buttons[6]->getText().setRotation(180.f);
	m_buttons[7]->getText().setRotation(135.f);

	setSize(sf::Vector2f(150.f, 150.f));
}

Navigation::~Navigation()
{
}

bool Navigation::processEvent(const sf::Event &event)
{
	for (auto &button : m_buttons)
		button->processEvent(event);

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
			m_buttons[i]->setActiveColor(sf::Color(0, 0, 0, 20));
			m_buttons[i]->setTextColor(sf::Color(0, 255, 0, 220));
			m_buttons[i]->setTextActiveColor(sf::Color(0, 255, 0, 255));
		} else {
			m_buttons[i]->setActiveColor(sf::Color::Transparent);
			m_buttons[i]->setTextColor(sf::Color(0, 0, 0, 20));
			m_buttons[i]->setTextActiveColor(sf::Color(0, 0, 0, 20));
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
	m_alpha = alpha;
	for (auto &button : m_buttons)
		button->setAlpha(alpha);
}

float Navigation::getAlpha() const
{
	return m_alpha;
}

void Navigation::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	auto spacing = round(m_size.y / 20.f);
	auto buttonSize = round((m_size.y - spacing*2) / 3.f);
	auto padding = m_buttons[0]->getPadding();

	int buttonIndex = 0;
	for (int i = 0; i < 9; ++i)
	{
		int x = i % 3;
		int y = i / 3;
		if (x == 1 && y == 1)
			continue;
		auto &button = m_buttons[buttonIndex];
		button->getText().setCharacterSize(0.8f * buttonSize);
		button->setContentSize(sf::Vector2f(buttonSize - padding.left - padding.width, buttonSize - padding.top - padding.height));
		button->setPosition((buttonSize + spacing) * x, (buttonSize + spacing) * y);
		++buttonIndex;
	}

	m_bounds.left = getPosition().x;
	m_bounds.top = getPosition().y;
	m_bounds.width = m_size.x;
	m_bounds.height = m_size.y;

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
