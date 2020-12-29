#include <NovelTea/States/StateIntro.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/System/FileInputStream.hpp>
#include <iostream>

namespace NovelTea
{

StateIntro::StateIntro(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
}

void StateIntro::render(sf::RenderTarget &target)
{
	target.clear(sf::Color::White);
	m_renderTexture.clear(sf::Color::White);
	m_renderTexture.draw(m_spriteLogo);
	m_renderTexture.display();

	target.draw(m_textBg);
	target.draw(m_sprite, sf::BlendAdd);
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

	auto format = TextFormat();
	format.size(0.02f * wi);
	m_textBg.setText(text, format);
	m_textBg.setSize(sf::Vector2f(0.95f * wi, h));
	m_textBg.setPosition(round((w - wi)/2 + 0.025f * w), 0.f);

	auto texture = AssetManager<sf::Texture>::get("images/noveltea.png");
	auto targetScale = 0.9f * wi / texture->getSize().x;
	m_spriteLogo.setTexture(*texture, true);
	m_spriteLogo.setPosition(w / 2, h / 2);
	m_spriteLogo.setOrigin(1.38f * m_spriteLogo.getLocalBounds().width / 2, m_spriteLogo.getLocalBounds().height / 2);
	m_spriteLogo.setScale(targetScale*35, targetScale*35);

	TweenEngine::Tween::to(m_spriteLogo, TweenSprite::SCALE_XY, 5.f)
		.target(targetScale, targetScale)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_spriteLogo, TweenSprite::ORIGIN_X, 1.f)
		.target(m_spriteLogo.getLocalBounds().width / 2)
		.delay(4.f)
		.start(m_tweenManager);

	m_textBg.setAlpha(0.f);
	TweenEngine::Tween::to(m_textBg, ActiveText::ALPHA, 5.f)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textBg, ActiveText::ALPHA, 2.f)
		.target(0.f)
		.delay(5.5f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_textBg, ActiveText::POSITION_Y, 5.f)
		.ease(TweenEngine::TweenEquations::easeInOutLinear)
		.target(0.f - wi * 1.1f)
		.repeat(-1, 0.f)
		.start(m_tweenManager);

	TweenEngine::Tween::mark()
		.delay(7.5f)
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
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
