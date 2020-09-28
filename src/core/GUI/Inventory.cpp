#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/Inventory.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/ObjectList.hpp>
#include <NovelTea/Object.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

Inventory::Inventory()
: m_needsUpdate(true)
, m_margin(3.f)
, m_itemHeight(30.f)
, m_alpha(255.f)
, m_isOpen(false)
, m_scrollPos(0.f)
, m_callback(nullptr)
{
	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_button.getText().setFont(*Proj.getFont(1));
	m_button.setTexture(texture.get());
	m_button.setString(L"\uf290");
	m_button.setActiveColor(sf::Color::Yellow);
	m_button.onClick([this](){
		if (isOpen())
			close();
		else
			open();
	});

	setSize(sf::Vector2f(150.f, 150.f));
}

void Inventory::update(float delta)
{
	m_scrollBar.update(delta);
	Hideable::update(delta);
}

bool Inventory::processEvent(const sf::Event &event)
{
	if (isOpen() && m_scrollBar.processEvent(event))
		return false;
	if (!m_button.processEvent(event))
		return false;
	if (!isOpen())
		return false;

	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		if (!m_bg.getGlobalBounds().contains(p))
		{
			close();
			return false;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		if (m_objectTexts.empty())
			return false;
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		auto posY = m_objectTexts[0]->getPosition().y + m_itemHeight;
		for (auto &t : m_objectTexts)
			t->setFillColor(sf::Color::Black);
		for (int i = 0; i < m_objectTexts.size(); ++i)
		{
			if (posY > p.y)
			{
				auto &text = m_objectTexts[i];
				text->setFillColor(sf::Color::Red);
				if (m_callback)
					m_callback(text->getString().toAnsiString(), m_bg.getPosition().x, posY);
				return true;
			}
			posY += m_itemHeight;
		}
	}
	return false;
}

void Inventory::open()
{
	if (isOpen())
		return;
	refreshItems();
	m_isOpen = true;
}

void Inventory::close()
{
	if (!isOpen())
		return;
	m_isOpen = false;
}

bool Inventory::isOpen()
{
	return m_isOpen;
}

void Inventory::refreshItems()
{
	auto maxWidth = 100.f;
	m_objectTexts.clear();
	for (auto &object : ActiveGame->getObjectList()->objects())
	{
		std::cout << object->getName() << std::endl;
		auto text = new TweenText;
		text->setString(object->getName());
		text->setFont(*Proj.getFont(0));
		text->setFillColor(sf::Color::Black);
		if (text->getLocalBounds().width > maxWidth)
			maxWidth = text->getLocalBounds().width;
		m_objectTexts.emplace_back(text);
	}

	m_button.setPosition(m_size.x - m_button.getSize().x - 3.f, m_size.y - m_button.getSize().y - 3.f);
	m_bg.setSize(sf::Vector2f(maxWidth + m_margin*2 + 4.f, 150.f));
	m_bg.setPosition(m_size.x - m_bg.getSize().x, m_button.getPosition().y - m_bg.getSize().y);
	m_scrollBar.setSize(sf::Vector2u(2, m_bg.getSize().y));
	m_scrollBar.setPosition(m_size.x - 4.f, m_bg.getPosition().y);
	m_scrollBar.setScrollAreaSize(sf::Vector2u(320, 150.f));
	repositionItems();
}

void Inventory::repositionItems()
{
	float posY = m_margin + m_scrollPos + m_bg.getPosition().y;
	for (auto &text : m_objectTexts)
	{
		text->setPosition(m_bg.getPosition().x + m_margin, posY);
		posY += m_itemHeight;
	}
	m_scrollAreaSize.y = posY - m_scrollPos - m_bg.getPosition().y + 10.f;
	updateScrollSize();
}

void Inventory::setScroll(float position)
{
//	if (m_scrollPos == position)
//		return;
	m_scrollPos = round(position);
	std::cout << "setScroll " << m_scrollPos << std::endl;
	repositionItems();
}

float Inventory::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &Inventory::getScrollSize()
{
	return m_scrollAreaSize;
}

void Inventory::setSize(const sf::Vector2f &size)
{
	m_needsUpdate = true;
	m_size = size;
	refreshItems();
}

sf::Vector2f Inventory::getSize() const
{
	return m_size;
}

sf::FloatRect Inventory::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect Inventory::getGlobalBounds() const
{
	ensureUpdate();
	return getTransform().transformRect(m_bounds);
}

void Inventory::setCallback(InventoryCallback callback)
{
	m_callback = callback;
}

void Inventory::setAlpha(float alpha)
{
	sf::Color color;
	m_alpha = alpha;
	float *newValues = &m_alpha;
//	for (int i = 0; i < m_buttons.size(); ++i)
//	{
//		float alphaMax = m_paths[i][0].ToBool() ? 255.f : 40.f;
//		SET_ALPHA(m_buttons[i]->getFillColor, m_buttons[i]->setFillColor, alphaMax);
//	}
	m_button.setAlpha(alpha);
}

float Inventory::getAlpha() const
{
	return m_alpha;
}

void Inventory::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	m_bounds.left = getPosition().x;
	m_bounds.top = getPosition().y;
	m_bounds.width = 36.f * 3;
	m_bounds.height = 36.f * 3;

	m_needsUpdate = false;
}

void Inventory::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	auto transform = getTransform();
	states.transform *= transform;
	target.draw(m_button, states);
	if (m_isOpen)
	{
		target.draw(m_bg, states);

		auto view = target.getView();
//		if (m_lastTransform != transform)
		{
			m_lastTransform = transform;
			auto bounds = m_bg.getGlobalBounds();

			sf::FloatRect viewport;
			viewport.left = bounds.left / view.getSize().x;
			viewport.top = bounds.top / view.getSize().y;
			viewport.width = bounds.width / view.getSize().x;
			viewport.height = bounds.height / view.getSize().y;

			m_view.reset(bounds);
			m_view.setViewport(viewport);
			m_scrollBar.setDragRect(bounds);
		}

		target.setView(m_view);

		for (auto &objectText : m_objectTexts)
			target.draw(*objectText, states);

		target.setView(view);

		target.draw(m_scrollBar, states);
	}
}

} // namespace NovelTea
