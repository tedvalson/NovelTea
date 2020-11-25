#include <NovelTea/States/StateTitleScreen.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateTitleScreen::StateTitleScreen(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	m_bg.setFillColor(sf::Color::White);
	m_text.setFont(*Proj.getFont(0));
	m_text.setString("Test");

	auto c = context.config.backgroundColor;
	TweenEngine::Tween::to(m_bg, TweenRectangleShape::FILL_COLOR_RGB, 2.f)
		.target(c.r, c.g, c.b)
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this](TweenEngine::BaseTween*){
			//
		})
		.start(m_tweenManager);

	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonStart.getText().setFont(*Proj.getFont(1));
	m_buttonStart.getText().setCharacterSize(50.f);
	m_buttonStart.setTexture(texture.get());
	m_buttonStart.setString(L"\uf290");
	m_buttonStart.setActiveColor(sf::Color(0, 0, 0, 50));
	m_buttonStart.setColor(sf::Color(0, 0, 0, 30));
	m_buttonStart.setAlpha(100.f);
	m_buttonStart.setPosition(200.f, 200.f);
	m_buttonStart.onClick([this](){
		if (m_tweenManager.getRunningTweensCount() > 0)
			return;
		TweenEngine::Tween::to(*this, ALPHA, 2.f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				requestStackClear();
				requestStackPush(StateID::Main);
			})
			.start(m_tweenManager);
	});

	setAlpha(0.f);
	TweenEngine::Tween::to(*this, ALPHA, 2.f)
		.target(255.f)
		.start(m_tweenManager);
}

void StateTitleScreen::render(sf::RenderTarget &target)
{
	target.clear(m_bg.getFillColor());
	target.draw(m_text);
	target.draw(m_buttonStart);
}

void StateTitleScreen::setAlpha(float alpha)
{
	m_buttonStart.setAlpha(alpha);
	m_alpha = alpha;
}

int StateTitleScreen::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case ALPHA:
		returnValues[0] = m_alpha;
		return 1;
	default:
		return -1;
	}
}

void StateTitleScreen::setValues(int tweenType, float *newValues)
{
	if (tweenType == ALPHA)
		setAlpha(newValues[0]);
}

bool StateTitleScreen::processEvent(const sf::Event &event)
{
	m_buttonStart.processEvent(event);
	return true;
}

bool StateTitleScreen::update(float delta)
{
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
