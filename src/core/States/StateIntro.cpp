#include <NovelTea/States/StateIntro.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/TextTypes.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/System/FileInputStream.hpp>
#include <iostream>

namespace NovelTea
{

StateIntro::StateIntro(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	m_textCopyright.setFont(*Proj.getFont());
	m_textCopyright.setString("NovelTea Engine");
}

void StateIntro::render(sf::RenderTarget &target)
{
	target.clear(sf::Color::White);
	m_renderTexture.clear(sf::Color::White);
	m_renderTexture.draw(m_spriteLogo);
	m_renderTexture.display();

	target.draw(m_textBg);
	target.draw(m_sprite, sf::BlendAdd);
	target.draw(m_textCopyright);
}

void StateIntro::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto wi = std::min(w, h);

	sf::FileInputStream file;
	std::string text;
#ifdef ANDROID
	if (file.open("intro.txt"))
#else
	if (file.open("/home/android/dev/NovelTea/res/assets/intro.txt"))
#endif
	{
		text.resize(file.getSize());
		file.read(&text[0], text.size());
	}

	m_tweenManager.killAll();

	m_renderTexture.create(w, h);
	m_sprite.setTexture(m_renderTexture.getTexture(), true);

	TextProperties textProps;
	textProps.fontSize = 0.016f * wi;
	m_textBg.setText(text, textProps);
	m_textBg.setSize(sf::Vector2f(0.95f * wi, h));
	m_textBg.setPosition(round((w - wi)/2 + 0.025f * w), 0.f);

	auto texture = AssetManager<sf::Texture>::get("images/noveltea.png");
	auto targetScale = 0.9f * wi / texture->getSize().x;
	m_spriteLogo.setTexture(*texture, true);
	m_spriteLogo.setPosition(w / 2, h / 2);
	m_spriteLogo.setOrigin(1.38f * m_spriteLogo.getLocalBounds().width / 2, m_spriteLogo.getLocalBounds().height / 2);
	m_spriteLogo.setScale(targetScale*35, targetScale*35);

	m_textCopyright.setCharacterSize(0.03f * wi);
	m_textCopyright.setFillColor(sf::Color(0,0,0,0));
	m_textCopyright.setPosition(
		round((w + m_spriteLogo.getLocalBounds().width * targetScale) / 2 - (1.1f * m_textCopyright.getLocalBounds().width)),
		round((h + m_spriteLogo.getLocalBounds().height * targetScale) / 2)
	);

	TweenEngine::Tween::to(m_spriteLogo, TweenSprite::SCALE_XY, 5.f)
		.target(targetScale, targetScale)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_spriteLogo, TweenSprite::ORIGIN_X, 1.f)
		.target(m_spriteLogo.getLocalBounds().width / 2)
		.delay(4.f)
		.start(m_tweenManager);

	TweenEngine::Tween::to(m_textCopyright, TweenText::FILL_COLOR_ALPHA, 1.f)
		.target(50.f)
		.delay(6.5f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textCopyright, TweenText::FILL_COLOR_ALPHA, 2.f)
		.target(0.f)
		.delay(8.0f)
		.start(m_tweenManager);

	m_textBg.setAlpha(0.f);
	TweenEngine::Tween::to(m_textBg, ActiveText::ALPHA, 5.f)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textBg, ActiveText::ALPHA, 2.f)
		.target(0.f)
		.delay(8.0f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textBg, ActiveText::POSITION_Y, 5.f)
		.ease(TweenEngine::TweenEquations::easeInOutLinear)
		.target(0.f - wi * 1.2f)
		.repeat(-1, 0.f)
		.start(m_tweenManager);

	TweenEngine::Tween::mark()
		.delay(10.0f)
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this](TweenEngine::BaseTween*){
			requestStackClear();
			requestStackPush(StateID::TitleScreen);
		})
		.start(m_tweenManager);
}

bool StateIntro::processEvent(const sf::Event &event)
{
	return true;
}

bool StateIntro::update(float delta)
{
	m_tweenManager.update(delta * 1.1f);
	return true;
}

} // namespace NovelTea
