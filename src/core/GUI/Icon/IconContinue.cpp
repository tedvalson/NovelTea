#include <NovelTea/GUI/Icon/IconContinue.hpp>
#include <NovelTea/ProjectData.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

IconContinue::IconContinue()
{
	m_text.setFont(*Proj.getFont("sysIcon"));
	m_text.setString(L"\uf0da");
	m_text.setFillColor(sf::Color(90, 90, 90));
	m_text.setOutlineColor(sf::Color(190, 190, 190));
	m_text.setOutlineThickness(1.f);
	reset();
}

void IconContinue::reset()
{
	auto color = m_text.getFillColor();
	color.a = 0;
	m_text.setFillColor(color);

	color = m_text.getOutlineColor();
	color.a = 0;
	m_text.setOutlineColor(color);

	m_text.setPosition(0.f, 0.f);
}

TweenText &IconContinue::getText()
{
	return m_text;
}

void IconContinue::show(float duration)
{
	m_tweenManager.killAll();
	reset();

	TweenEngine::Tween::to(m_text, TweenText::FILL_COLOR_ALPHA, duration)
		.target(255.f)
		.delay(0.5f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, TweenText::OUTLINE_COLOR_ALPHA, duration)
		.target(255.f)
		.delay(0.5f)
		.start(m_tweenManager);

	TweenEngine::Tween::to(m_text, TweenText::POSITION_X, 1.f)
		.targetRelative(0.3f * m_text.getCharacterSize())
		.ease(TweenEngine::TweenEquations::easeOutBounce)
		.repeatYoyo(-1, 0.f)
		.start(m_tweenManager);
}

void IconContinue::hide(float duration)
{
	m_tweenManager.killAll();
	TweenEngine::Tween::to(m_text, TweenText::FILL_COLOR_ALPHA, duration)
		.target(0.f)
			.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, TweenText::OUTLINE_COLOR_ALPHA, duration)
		.target(0.f)
			.start(m_tweenManager);
}

void IconContinue::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_text, states);
}

void IconContinue::update(float delta)
{
	m_tweenManager.update(delta);
}


} // namespace NovelTea
