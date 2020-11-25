#include <NovelTea/States/StateTitleScreen.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateTitleScreen::StateTitleScreen(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	auto width = context.config.width;
	auto height = context.config.height;

	// Title
	TextFormat format;
	format.size(35);
	format.bold(true);
	m_textTitle.setSize(sf::Vector2f(0.95f * width, height));
	m_textTitle.setText(ProjData[ID::projectName].ToString(), format);
	m_textTitle.setPosition(0.05f * width, 0.05f * width);

	// Author
	format.color(sf::Color(120, 120, 120));
	format.size(8);
	format.bold(false);
	m_textAuthor.setSize(sf::Vector2f(0.9f * width, height));
	m_textAuthor.setPosition(0.05 * width, (0.1f * width) + m_textTitle.getLocalBounds().height);
	m_textAuthor.setText("created by " + ProjData[ID::projectAuthor].ToString(), format);

	// Buttons
	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png");
	m_buttonStart.getText().setFont(*Proj.getFont(0));
	m_buttonStart.getText().setCharacterSize(45.f);
	m_buttonStart.setTexture(texture.get());
	m_buttonStart.setString("Start");
	m_buttonStart.setTextColor(sf::Color(80, 80, 80));
	m_buttonStart.setActiveColor(sf::Color(0, 0, 0, 50));
	m_buttonStart.setColor(sf::Color(0, 0, 0, 30));
	m_buttonStart.setContentSize(300.f, 60.f);
	m_buttonStart.setPosition(0.5f * (width - 300.f), height - 200.f);
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

	m_buttonSettings = m_buttonStart;
	m_buttonSettings.setString("Settings");
	m_buttonSettings.setPosition(0.5f * (width - 300.f), height - 120.f);
	m_buttonSettings.onClick([this](){
		//
	});

	setAlpha(0.f);
	TweenEngine::Tween::to(*this, ALPHA, 2.f)
		.target(255.f)
		.start(m_tweenManager);

	auto c = context.config.backgroundColor;
	TweenEngine::Tween::to(m_bg, TweenRectangleShape::FILL_COLOR_RGB, 2.f)
		.target(c.r, c.g, c.b)
		.start(m_tweenManager);
}

void StateTitleScreen::render(sf::RenderTarget &target)
{
	target.clear(m_bg.getFillColor());
	target.draw(m_textTitle);
	target.draw(m_textAuthor);
	target.draw(m_buttonSettings);
	target.draw(m_buttonStart);
}

void StateTitleScreen::setAlpha(float alpha)
{
	m_textTitle.setAlpha(alpha);
	m_textAuthor.setAlpha(alpha);
	m_buttonStart.setAlpha(alpha);
	m_buttonSettings.setAlpha(alpha);
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
	m_buttonSettings.processEvent(event);
	return true;
}

bool StateTitleScreen::update(float delta)
{
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
