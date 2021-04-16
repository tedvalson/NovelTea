#include <NovelTea/States/StateSettings.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateSettings::StateSettings(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	auto &defaultFont = *Proj.getFont(0);
	m_textTitle.setFont(defaultFont);
	m_textTitle.setString("Settings");
	m_buttonBack.setString("Back");
	m_buttonTextSize.setString("Text Size");

	m_buttonTextSize.onClick([this](){
		requestStackPush(StateID::TextSettings);
	});
	m_buttonBack.onClick([this](){
		close();
	});

	m_bg.setFillColor(sf::Color(180, 180, 180));

	hide(0.f);
	show(0.5f, ALPHA, [this](){
		//
	});
}

void StateSettings::render(sf::RenderTarget &target)
{
	target.draw(m_bg);
	target.draw(m_textTitle);
	target.draw(m_buttonTextSize);
	target.draw(m_buttonBack);
}

void StateSettings::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto portrait = (h > w);

	m_bg.setSize(size);

	m_buttonTextSize.setPosition(10.f, 100.f);
	m_buttonBack.setPosition(10.f, 200.f);
}

void StateSettings::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_textTitle.getFillColor, m_textTitle.setFillColor, 245.f);
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 235.f);
	m_buttonTextSize.setAlpha(alpha);
	m_buttonBack.setAlpha(alpha);
	State::setAlpha(alpha);
}

bool StateSettings::processEvent(const sf::Event &event)
{
	if (isShowing() || isHiding())
		return false;

	m_buttonTextSize.processEvent(event);
	m_buttonBack.processEvent(event);
	return false;
}

bool StateSettings::update(float delta)
{
	m_tweenManager.update(delta);
	Hideable::update(delta);
	return false;
}

} // namespace NovelTea
