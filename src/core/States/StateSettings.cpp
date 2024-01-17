#include <NovelTea/States/StateSettings.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/SFML/AssetLoaderSFML.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateSettings::StateSettings(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_buttonBack(&context)
, m_buttonTextSize(&context)
{
	auto &defaultFont = *Asset->font();
	m_textTitle.setFont(defaultFont);
	m_textTitle.setString("Settings");
	m_textTitle.setFillColor(sf::Color::Black);

	m_buttonBack.getText().setFont(*Asset->font("sysIcon"));
	m_buttonBack.setString(L"\uf00d");

	m_buttonTextSize.setString("Text Size");

	m_buttonBack.setColor(sf::Color(230, 0 , 0));
	m_buttonBack.setTextColor(sf::Color::White);

	m_bg.setTexture(m_buttonBack.getTexture());

	m_buttonTextSize.onClick([this](){
		close(0.5f, StateID::TextSettings);
	});
	m_buttonBack.onClick([this](){
		close();
	});

	m_overlay.setFillColor(sf::Color::Black);
	m_bg.setColor(sf::Color(180, 180, 180));

	hide(0.f);
	show(0.5f, ALPHA, [this](){
		//
	});
}

void StateSettings::render(sf::RenderTarget &target)
{
	target.draw(m_overlay);
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

	m_buttonTextSize.getText().setCharacterSize(buttonFontSize);
	m_buttonTextSize.setSize(buttonWidth, buttonHeight);
	m_buttonTextSize.setPosition(round((w - buttonWidth)/2), round(bgY + bgHeight - buttonHeight - margin));

	m_buttonBack.getText().setCharacterSize(buttonHeight * 0.55f);
	m_buttonBack.setSize(buttonHeight * 0.6f, buttonHeight * 0.6f);
	m_buttonBack.setPosition(bgX + bgWidth - m_buttonBack.getSize().x * 0.7f, bgY - m_buttonBack.getSize().y * 0.3f);
}

void StateSettings::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_textTitle.getFillColor, m_textTitle.setFillColor, 255.f);
	SET_ALPHA(m_overlay.getFillColor, m_overlay.setFillColor, 150.f);
	SET_ALPHA(m_bg.getColor, m_bg.setColor, 255.f);
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
