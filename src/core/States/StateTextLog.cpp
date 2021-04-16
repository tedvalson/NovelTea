#include <NovelTea/States/StateTextLog.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateTextLog::StateTextLog(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	m_text.setFont(*Proj.getFont(0));
	m_text.setString("Text log");

	m_bg.setSize(sf::Vector2f(context.config.width, context.config.height));
	m_bg.setFillColor(sf::Color(180, 180, 180, 0));

	setAlpha(0.f);
	TweenEngine::Tween::to(*this, ALPHA, 1.f)
		.target(240.f)
		.start(m_tweenManager);
}

void StateTextLog::render(sf::RenderTarget &target)
{
	target.draw(m_bg);
	target.draw(m_text);
}

void StateTextLog::resize(const sf::Vector2f &size)
{
}

void StateTextLog::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_text.getFillColor, m_text.setFillColor, 245.f);
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
	State::setAlpha(alpha);
}

bool StateTextLog::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (m_tweenManager.getRunningTweensCount() > 0)
			return false;
		TweenEngine::Tween::to(*this, ALPHA, 2.f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				requestStackPop();
			})
			.start(m_tweenManager);
	}
	return false;
}

bool StateTextLog::update(float delta)
{
	m_tweenManager.update(delta);
	return false;
}

} // namespace NovelTea
