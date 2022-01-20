#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/Inventory.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/ObjectList.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/ProjectData.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

Inventory::Inventory()
: m_needsUpdate(true)
, m_margin(0.f)
, m_itemHeight(20.f)
, m_fontSizeMultiplier(1.f)
, m_alpha(255.f)
, m_isOpen(false)
, m_scrollPos(0.f)
, m_callback(nullptr)
{
	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	setScreenSize(sf::Vector2f(720.f, 1280.f));
}

bool Inventory::update(float delta)
{
	m_scrollBar.update(delta);
	Hideable::update(delta);
	return true;
}

bool Inventory::processEvent(const sf::Event &event)
{
	if (!isOpen())
		return false;
	if (m_scrollBar.processEvent(event))
		return true;

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
					m_callback(m_objectIds[i], m_bg.getPosition().x, posY);
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
	auto width = 0.f;
	auto height = 0.f;
	m_objectTexts.clear();
	m_objectIds.clear();
	for (auto &object : ActiveGame->getObjectList()->objects())
	{
		auto text = new TweenText;
		text->setString(object->getName());
		text->setFont(*Proj.getFont(0));
		text->setFillColor(sf::Color::Black);
		text->setCharacterSize(m_itemHeight);
		if (text->getLocalBounds().width > width)
			width = text->getLocalBounds().width;
		height += m_itemHeight;
		m_objectTexts.emplace_back(text);
		m_objectIds.emplace_back(object->getId());
	}

	height = std::min(height, 50.f);

	m_bg.setSize(sf::Vector2f(std::min(width, m_screenSize.x) + m_margin*2 + 4.f, height + m_margin*2));
	m_bg.setPosition(m_startPosition.x - m_bg.getSize().x, m_startPosition.y - m_bg.getSize().y);
	m_scrollBar.setSize(sf::Vector2f(2, m_bg.getSize().y));
	m_scrollBar.setPosition(m_startPosition.x - 4.f, m_bg.getPosition().y);
	m_scrollBar.setScrollAreaSize(sf::Vector2f(0, height));

	m_scrollAreaSize.y = m_itemHeight * m_objectTexts.size();
	updateScrollbar();
	repositionItems();
}

void Inventory::repositionItems()
{
	auto posX = round(m_bg.getPosition().x + m_margin);
	auto posY = m_margin + m_scrollPos + m_bg.getPosition().y;
	for (auto &text : m_objectTexts)
	{
		text->setPosition(posX, round(posY));
		posY += m_itemHeight;
	}
}

void Inventory::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
	m_itemHeight = 26.f * fontSizeMultiplier;
	m_margin = 0.3f * m_itemHeight;
	refreshItems();
	m_scrollBar.setScroll(0.f);
}

void Inventory::setScreenSize(const sf::Vector2f &size)
{
	m_needsUpdate = true;
	m_screenSize = size;
	refreshItems();
	m_scrollBar.setScroll(0.f);
}

void Inventory::setScroll(float position)
{
//	if (m_scrollPos == position)
//		return;
	m_scrollPos = round(position);
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

void Inventory::setStartPosition(const sf::Vector2f &position)
{
	m_startPosition = position;
}

void Inventory::setAlpha(float alpha)
{
	m_alpha = alpha;
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
