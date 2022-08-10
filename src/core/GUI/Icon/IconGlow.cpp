#include <NovelTea/GUI/Icon/IconGlow.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ProjectData.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

IconGlow::IconGlow(Context *context)
	: ContextObject(context)
	, m_speedFactor(1.f)
{
	m_text.setFont(*Proj->getFont("sysIcon"));
	m_text.setString(L"\uf138");
	reset();
}

void IconGlow::reset()
{
	auto color = m_text.getFillColor();
	color.a = 0;
	m_text.setOrigin(m_text.getLocalBounds().width / 2, m_text.getLocalBounds().height / 2);
	m_text.setFillColor(color);
	m_text.setScale(1.f, 1.f);
}

void IconGlow::setSpeed(float factor)
{
	m_speedFactor = factor;
}

TweenText &IconGlow::getText()
{
	return m_text;
}

void IconGlow::show(float duration, float expiration)
{
	m_tweenManager.killAll();
	reset();

	TweenEngine::Tween::to(m_text, TweenText::FILL_COLOR_ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);

	if (expiration > 0.f)
		TweenEngine::Tween::mark()
			.delay(expiration)
			.setCallback(TweenEngine::TweenCallback::BEGIN, [this](TweenEngine::BaseTween*){
				hide();
			}).start(m_tweenManager);

	TweenEngine::Tween::to(m_text, TweenText::SCALE_XY, 1.f / m_speedFactor)
		.target(1.2f, 1.2f)
		.repeatYoyo(-1, 0.f)
		.start(m_tweenManager);
}

void IconGlow::hide(float duration)
{
	m_tweenManager.killAll();
	TweenEngine::Tween::to(m_text, TweenText::FILL_COLOR_ALPHA, duration)
		.target(0.f)
			.start(m_tweenManager);
}

void IconGlow::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_text, states);
}

void IconGlow::update(float delta)
{
	m_tweenManager.update(delta);
}


} // namespace NovelTea
