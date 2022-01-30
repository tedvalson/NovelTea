#include <NovelTea/States/StateProfileManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/Settings.hpp>
#include <SFML/Graphics/View.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateProfileManager::StateProfileManager(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_scrollPos(0.f)
, m_buttonHeight(40.f)
, m_buttonSpacing(0.f)
, m_cancelButtonPress(false)
{
	auto &defaultFont = *Proj.getFont(0);
	m_textTitle.setFont(defaultFont);
	m_textTitle.setString("Profiles");
	m_textTitle.setFillColor(sf::Color::Black);

	m_buttonBack.getText().setFont(*Proj.getFont(1));
	m_buttonBack.setString(L"\uf00d");
	m_buttonBack.setColor(sf::Color(230, 0 , 0));
	m_buttonBack.setTextColor(sf::Color::White);

	m_buttonAdd.getText().setFont(*Proj.getFont(1));
	m_buttonAdd.setString(L"\uf234");
	m_buttonAdd.setColor(sf::Color(0, 0 , 0));
	m_buttonAdd.setTextColor(sf::Color::White);

	m_buttonRemove = m_buttonAdd;
	m_buttonRemove.setString(L"\uf1f8");

	m_bg.setTexture(m_buttonBack.getTexture());

	m_buttonBack.onClick([this](){
		if (m_cancelButtonPress)
			return;
		GSettings.save();
		close();
	});
	m_buttonAdd.onClick([this](){
		if (!m_cancelButtonPress)
			add();
	});
	m_buttonRemove.onClick([this](){
		if (!m_cancelButtonPress)
			remove();
	});

	m_overlay.setFillColor(sf::Color::Black);
	m_bg.setColor(sf::Color(180, 180, 180));

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	hide(0.f);
	show(0.5f, ALPHA, [this](){
//		refresh();
	});
}

void StateProfileManager::render(sf::RenderTarget &target)
{
	target.draw(m_overlay);
	target.draw(m_bg);
	target.draw(m_textTitle);
	target.draw(m_buttonAdd);
	target.draw(m_buttonRemove);
	target.draw(m_buttonBack);

	auto view = target.getView();
	target.setView(m_view);
	for (auto &button : m_profileButtons)
		target.draw(*button);
	target.setView(view);

	target.draw(m_scrollBar);
}

void StateProfileManager::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto portrait = (h > w);

	auto bgWidth = (portrait ? 0.85f : 0.45f) * w;
	auto bgHeight = (portrait ? 0.5f : 0.8f) * h;
	auto bgX = (w - bgWidth) / 2;
	auto bgY = (h - bgHeight) / 2;
	auto margin = bgWidth * 0.05f;
	auto buttonWidth = bgWidth - margin * 2;
	auto buttonHeight = (portrait ? 0.09f : 0.12f) * h;
	auto buttonFontSize = buttonHeight * 0.7f;

	m_overlay.setSize(size);

	m_bg.setSize(bgWidth, bgHeight);
	m_bg.setPosition(bgX, bgY);

	m_textTitle.setCharacterSize(1.1f * buttonFontSize);
	m_textTitle.setOrigin(m_textTitle.getLocalBounds().width / 2, 0.f);
	m_textTitle.setPosition(round(0.5f * w), round(bgY + margin));

	m_buttonBack.getText().setCharacterSize(buttonHeight * 0.55f);
	m_buttonBack.setSize(buttonHeight * 0.6f, buttonHeight * 0.6f);
	m_buttonBack.setPosition(bgX + bgWidth - m_buttonBack.getSize().x * 0.7f, bgY - m_buttonBack.getSize().y * 0.3f);

	m_buttonAdd.getText().setCharacterSize(buttonHeight * 0.5f);
	m_buttonAdd.setSize(buttonHeight * 0.8f, buttonHeight * 0.8f);
	m_buttonAdd.setPosition(bgX, bgY);

	m_buttonRemove.getText().setCharacterSize(buttonHeight * 0.5f);
	m_buttonRemove.setSize(buttonHeight * 0.8f, buttonHeight * 0.8f);
	m_buttonRemove.setPosition(bgX, bgY + buttonHeight);

	sf::FloatRect viewport;
	auto titleBounds = m_textTitle.getGlobalBounds();
	auto titleBottom = titleBounds.top + titleBounds.height + margin;
	m_buttonRect = sf::FloatRect(bgX, titleBottom, bgWidth, bgHeight + bgY - titleBottom);
	viewport.left = m_buttonRect.left / w;
	viewport.top = m_buttonRect.top / h;
	viewport.width = m_buttonRect.width / w;
	viewport.height = m_buttonRect.height / h;
	m_view.reset(m_buttonRect);
	m_view.setViewport(viewport);

//	m_bounds = sf::FloatRect(0.f, 0.f, maxWidth + m_margin*2, posY + m_margin*2);
	m_scrollBar.setSize(sf::Vector2f(2, bgHeight));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(0, m_buttonRect.height));
	m_scrollBar.setPosition(bgX + bgWidth, bgY);
	m_scrollBar.setDragRect(m_buttonRect);

	refresh();
}

void StateProfileManager::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionItems();
}

float StateProfileManager::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &StateProfileManager::getScrollSize()
{
	return m_scrollAreaSize;
}

void StateProfileManager::repositionItems()
{
	auto posX = m_buttonRect.left + m_bg.getSize().x * 0.15f;
	auto posY = m_buttonRect.top + m_scrollPos;
	for (auto &button : m_profileButtons)
	{
		button->setPosition(posX, posY);
		posY += m_buttonHeight + m_buttonSpacing;
	}

}

void StateProfileManager::setActiveProfile(int index)
{
	for (auto &button : m_profileButtons)
	{
		button->setColor(sf::Color::White);
	}
	m_profileButtons[index]->setColor(sf::Color(100, 100, 255));
	GSettings.setActiveProfileIndex(index);
	runCallback(&index);
}

void StateProfileManager::add()
{
	GSettings.addProfile();
	refresh();
}

void StateProfileManager::remove()
{
	GSettings.removeProfile(GSettings.getActiveProfileIndex());
	refresh();
}

void StateProfileManager::refresh()
{
	m_buttonHeight = m_bg.getSize().y * 0.2f;
	m_buttonSpacing = m_buttonHeight * 0.1f;
	m_profileButtons.clear();
	int i = 0;
	for (auto &profile : GSettings.getProfiles())
	{
		auto button = new Button;
		button->getText().setCharacterSize(m_buttonHeight * 0.7f);
		button->setString("Profile " + std::to_string(i+1));
		button->setSize(m_bg.getSize().x * 0.7f, m_buttonHeight);
		button->setActiveColor(sf::Color::Blue);
		button->onClick([this, i](){
			if (!m_cancelButtonPress)
				setActiveProfile(i);
		});
		m_profileButtons.emplace_back(button);
		++i;
	}

	setActiveProfile(GSettings.getActiveProfileIndex());

	m_scrollAreaSize.y = (m_buttonHeight + m_buttonSpacing) * m_profileButtons.size();
	updateScrollbar();
	repositionItems();
	m_scrollBar.setScroll(0.f);
//	setScroll(0.f);
}

void StateProfileManager::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_textTitle.getFillColor, m_textTitle.setFillColor, 255.f);
	SET_ALPHA(m_overlay.getFillColor, m_overlay.setFillColor, 150.f);
	SET_ALPHA(m_bg.getColor, m_bg.setColor, 255.f);
	m_buttonAdd.setAlpha(alpha);
	m_buttonRemove.setAlpha(alpha);
	m_buttonBack.setAlpha(alpha);
	for (auto &button : m_profileButtons)
		button->setAlpha(alpha);
	State::setAlpha(alpha);
}

bool StateProfileManager::processEvent(const sf::Event &event)
{
	if (isShowing() || isHiding())
		return false;
	m_scrollBar.processEvent(event);
	if (!m_cancelButtonPress && m_scrollBar.isScrolling())
		m_cancelButtonPress = true;

	m_buttonAdd.processEvent(event);
	m_buttonRemove.processEvent(event);
	m_buttonBack.processEvent(event);
	for (auto &button : m_profileButtons)
		button->processEvent(event);

	if (event.type == sf::Event::MouseButtonReleased)
		m_cancelButtonPress = false;
	return false;
}

bool StateProfileManager::update(float delta)
{
	m_tweenManager.update(delta);
	m_scrollBar.update(delta);
	Hideable::update(delta);
	return false;
}

} // namespace NovelTea
