#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/SaveData.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

VerbList::VerbList()
: m_scrollPos(0.f)
, m_margin(0.f)
, m_itemHeight(38.f)
, m_screenSize(320.f, 400.f)
, m_selectCallback(nullptr)
, m_showHideCallback(nullptr)
{
	sf::FloatRect viewport(0.f, 0.f, 0.5f, 1.f);
	m_view.reset(sf::FloatRect(0, 0, 500, 700));
	m_view.setViewport(viewport);

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	auto bgColor = sf::Color(230, 230, 230, 0);
	m_bg.setFillColor(bgColor);

	std::vector<std::string> verbs;
	setVerbs(verbs);

	setAlpha(0.f);
}

bool VerbList::update(float delta)
{
	m_scrollBar.update(delta);
	Hideable::update(delta);
	return true;
}

bool VerbList::processEvent(const sf::Event &event)
{
	if (!isVisible() || m_verbs.empty() || isHiding())
		return false;
	if (m_scrollBar.processEvent(event))
		return true;

	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		// Needs to return true so inventory menu knows to stay open
		if (m_bg.getGlobalBounds().contains(p))
			return true;
		hide();
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		if (!m_bg.getGlobalBounds().contains(p))
			return false;

		auto posY = m_verbs[0].text.getPosition().y + m_itemHeight;
		for (auto i = 0; i < m_verbs.size(); ++i)
		{
			if (posY > p.y)
			{
				if (m_selectCallback)
					m_selectCallback(m_verbs[i].verbId);
				return true;
			}
			posY += m_itemHeight;
		}
	}

	return false;
}

void VerbList::refreshItems()
{
	float maxWidth = 0.f;
	const size_t maxItemsShown = 6;
	float posY = m_itemHeight * std::min(m_verbs.size(), maxItemsShown);
	posY = std::min(posY, 0.5f * m_screenSize.y);
	for (auto &verb : m_verbs) {
		verb.text.setCharacterSize(m_itemHeight);
		maxWidth = std::max(maxWidth, verb.text.getLocalBounds().width);
	}

	m_bounds = sf::FloatRect(0.f, 0.f, maxWidth + m_margin*2, posY + m_margin*2);
	m_scrollBar.setSize(sf::Vector2f(2, posY + m_margin*2));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(320, posY));
	m_scrollBar.setPosition(m_bounds.width - 4.f, 0.f);
	m_scrollBar.setDragRect(getGlobalBounds());
	m_scrollAreaSize.y = m_itemHeight * m_verbs.size();

	m_bg.setSize(sf::Vector2f(m_bounds.width, m_bounds.height));
	repositionItems();
	updateScrollbar();
	m_scrollBar.setScroll(0.f);
}

void VerbList::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_itemHeight = 26.f * fontSizeMultiplier;
	m_margin = 0.3f * m_itemHeight;
	m_fontSizeMultiplier = fontSizeMultiplier;
	refreshItems();
}

void VerbList::setScreenSize(const sf::Vector2f &size)
{
	m_screenSize = size;
	refreshItems();
}

const sf::Vector2f &VerbList::getScreenSize() const
{
	return m_screenSize;
}

void VerbList::show(float duration, int tweenType, HideableCallback callback)
{
	Hideable::show(duration, tweenType, [this, callback](){
		if (m_showHideCallback)
			m_showHideCallback(true);
		if (callback)
			callback();
	});
}

void VerbList::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, [this, callback](){
		if (m_showHideCallback)
			m_showHideCallback(false);
		if (callback)
			callback();
	});
}

void VerbList::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 230.f);
	SET_ALPHA(m_scrollBar.getColor, m_scrollBar.setColor, 40.f);
	for (auto &verb : m_verbs) {
		SET_ALPHA(verb.text.getFillColor, verb.text.setFillColor, 255.f);
	}
}

float VerbList::getAlpha() const
{
	return m_bg.getFillColor().a / 230.f * 255.f;
}

void VerbList::setVerbs(const std::vector<std::string> &verbIds)
{
	m_verbs.clear();

	for (auto &verbId : verbIds)
		addVerbOption(verbId);

	refreshItems();
}

void VerbList::setVerbs(const std::string &objectId)
{
	std::vector<std::string> verbIds;

	for (auto &item : GSave->data()[Verb::id].ObjectRange())
		verbIds.push_back(item.first);
	for (auto &item : ProjData[Verb::id].ObjectRange())
	{
		auto verbId = item.first;
		if (!GSave->data()[Verb::id].hasKey(verbId)) {
			verbIds.push_back(verbId);
		}
	}

	for (auto it = verbIds.begin(); it != verbIds.end();)
	{
		auto verb = GSave->get<Verb>(*it);
		if (!verb->checkConditionScript(*it, objectId))
			verbIds.erase(it);
		else
			++it;
	}

	setVerbs(verbIds);
}

void VerbList::setSelectCallback(VerbSelectCallback callback)
{
	m_selectCallback = callback;
}

void VerbList::setShowHideCallback(VerbShowHideCallback callback)
{
	m_showHideCallback = callback;
}

VerbSelectCallback VerbList::getSelectCallback() const
{
	return m_selectCallback;
}

VerbShowHideCallback VerbList::getShowHideCallback() const
{
	return m_showHideCallback;
}

void VerbList::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionItems();
}

float VerbList::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &VerbList::getScrollSize()
{
	return m_scrollAreaSize;
}

void VerbList::setPositionBounded(const sf::Vector2f &position)
{
	auto p = position;
	if (p.x + m_bounds.width > m_screenSize.x)
		p.x = m_screenSize.x - m_bounds.width;
	if (p.y + m_bounds.height > m_screenSize.y)
		p.y = m_screenSize.y - m_bounds.height;
	if (p.x < 0.f)
		p.x = 0.f;
	if (p.y < 0.f)
		p.y = 0.f;
	setPosition(p);
	m_scrollBar.setDragRect(getGlobalBounds());
}

sf::FloatRect VerbList::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect VerbList::getGlobalBounds() const
{
	return getTransform().transformRect(m_bounds);
}

void VerbList::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	auto transform = getTransform();
	states.transform *= transform;
	target.draw(m_bg, states);
	target.draw(m_scrollBar, states);

	auto view = target.getView();
//	if (m_lastTransform != transform)
	{
		m_lastTransform = transform;
		auto bounds = getGlobalBounds();

		sf::FloatRect viewport;
		viewport.left = bounds.left / view.getSize().x;
		viewport.top = bounds.top / view.getSize().y;
		viewport.width = bounds.width / view.getSize().x;
		viewport.height = bounds.height / view.getSize().y;

		m_view.reset(bounds);
		m_view.setViewport(viewport);
	}

	target.setView(m_view);

	for (auto &verb : m_verbs)
	{
		target.draw(verb.text, states);
	}

	target.setView(view);
}

void VerbList::repositionItems()
{
	float posY = m_margin + m_scrollPos;
	for (auto &verb : m_verbs)
	{
		verb.text.setPosition(m_margin, posY);
		posY += m_itemHeight;
	}
//	m_size.y = posY - m_scrollPos;
}

void VerbList::addVerbOption(const std::string &verbId)
{
	VerbOption option;
	option.verbId = verbId;
	option.text.setFillColor(sf::Color::Black);
	option.text.setFont(*Proj.getFont(0));

	auto verb = GSave->get<Verb>(verbId);
	option.text.setString(verb->getName());
	m_verbs.push_back(option);
}

} // namespace NovelTea
