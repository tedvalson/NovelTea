#include <NovelTea/States/StateTextSettings.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Settings.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateTextSettings::StateTextSettings(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	auto &defaultFont = *Proj.getFont(0);
	m_textTitle.setFont(defaultFont);
	m_textTitle.setString("Text Settings");

	m_textValue.setFont(defaultFont);

	std::string s;
	for (int i = 0; i < 5; ++i) {
		s += "\t";
		for (int j = 0; j < 5; ++j)
			s += "This is some sample text. Please use the arrows below to adjust the font size. ";
		s += "\n";
	}
	m_roomActiveText.setText(s);

	m_buttonFinish.getText().setFont(*Proj.getFont(1));
	m_buttonFinish.setString(L"\uf00c");
	m_buttonCancel = m_buttonFinish;
	m_buttonCancel.setString(L"\uf00d");
	m_buttonSizeDec = m_buttonFinish;
	m_buttonSizeDec.setString(L"\uf053");
	m_buttonSizeInc = m_buttonFinish;
	m_buttonSizeInc.setString(L"\uf054");

	m_buttonSizeInc.onClick([this](){
		changeSizeMultiplier(m_multiplier + 0.1f);
	});
	m_buttonSizeDec.onClick([this](){
		changeSizeMultiplier(m_multiplier - 0.1f);
	});
	m_buttonCancel.onClick([this](){
		getContext().config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
		getStack().resize(sf::Vector2f(getContext().config.width, getContext().config.height));
		close(0.5f, StateID::Settings);
	});
	m_buttonFinish.onClick([this](){
		GSettings.setFontSizeMultiplier(m_multiplier);
		GSettings.save();
		close(0.5f, StateID::Settings);
	});

	m_bg.setFillColor(sf::Color(180, 180, 180));
	m_toolbarBg.setFillColor(sf::Color(200, 200, 200));

	changeSizeMultiplier(context.config.fontSizeMultiplier);

	hide(0.f);
	show(0.5f);
}

void StateTextSettings::render(sf::RenderTarget &target)
{
//	target.draw(m_textTitle);
	target.draw(m_toolbarBg);
	target.draw(m_textValue);
	target.draw(m_buttonCancel);
	target.draw(m_buttonFinish);
	target.draw(m_buttonSizeDec);
	target.draw(m_buttonSizeInc);
}

void StateTextSettings::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto portrait = (h > w);
	auto buttonHeight = portrait ? 0.1f * w : 0.1f * h;
	auto buttonSize = sf::Vector2f(buttonHeight, buttonHeight);
	auto buttonTextSize = 0.8f * buttonHeight;

	m_bg.setSize(size);

	m_toolbarBg.setSize(sf::Vector2f(w, 1.5f * buttonHeight));
	m_toolbarBg.setPosition(0.f, h - m_toolbarBg.getSize().y);

	// Copy same sizing rules as StateMain::resize() for room text
	float roomTextPadding = round(1.f / 16.f * std::min(w, h));
	m_roomActiveText.setFontSizeMultiplier(m_multiplier);
	m_roomActiveText.setLineSpacing(m_multiplier * 5.f);
	m_roomActiveText.setSize(sf::Vector2f((portrait ? 1.f : 0.6f) * w - roomTextPadding*2, 0.f));
	m_roomActiveText.setPosition((w - m_roomActiveText.getSize().x)/2, roomTextPadding);

	m_buttonCancel.getText().setCharacterSize(buttonTextSize);
	m_buttonCancel.setSize(buttonSize);
	m_buttonCancel.setPosition(w - buttonSize.x * 1.2f, h - buttonSize.y * 1.2f);

	m_buttonFinish.getText().setCharacterSize(buttonTextSize);
	m_buttonFinish.setSize(buttonSize);
	m_buttonFinish.setPosition(m_buttonCancel.getPosition());
	m_buttonFinish.move(-m_buttonFinish.getSize().x, 0.f);

	m_buttonSizeInc.getText().setCharacterSize(buttonTextSize);
	m_buttonSizeInc.setSize(buttonSize);
	m_buttonSizeInc.setPosition(m_buttonFinish.getPosition());
	m_buttonSizeInc.move(-m_buttonSizeInc.getSize().x * 2, 0.f);

	m_textValue.setCharacterSize(buttonTextSize);
	m_textValue.setPosition(m_buttonSizeInc.getPosition());
	m_textValue.move(-m_textValue.getLocalBounds().width - 0.2f * buttonHeight, 0.f);

	m_buttonSizeDec.getText().setCharacterSize(buttonTextSize);
	m_buttonSizeDec.setSize(buttonSize);
	m_buttonSizeDec.setPosition(m_textValue.getPosition());
	m_buttonSizeDec.move(-m_buttonSizeDec.getSize().x - 0.2f * buttonHeight, 0.f);
}

void StateTextSettings::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_textTitle.getFillColor, m_textTitle.setFillColor, 245.f);
	SET_ALPHA(m_textValue.getFillColor, m_textValue.setFillColor, 245.f);
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
	m_buttonCancel.setAlpha(alpha);
	m_buttonFinish.setAlpha(alpha);
	m_buttonSizeDec.setAlpha(alpha);
	m_buttonSizeInc.setAlpha(alpha);
	m_roomActiveText.setAlpha(alpha);
	State::setAlpha(alpha);
}

void StateTextSettings::changeSizeMultiplier(float multiplier)
{
	m_multiplier = std::max(0.1f, multiplier);
	m_multiplier = std::min(4.0f, m_multiplier);

	std::string str;
	str.resize(4);
	std::snprintf(&str[0], str.size()+1, "x%0.1f", m_multiplier);
	m_textValue.setString(str);

	getContext().config.fontSizeMultiplier = m_multiplier;
	getStack().resize(sf::Vector2f(getContext().config.width, getContext().config.height));
}

bool StateTextSettings::processEvent(const sf::Event &event)
{
	if (isShowing() || isHiding())
		return false;
	m_buttonCancel.processEvent(event);
	m_buttonFinish.processEvent(event);
	m_buttonSizeDec.processEvent(event);
	m_buttonSizeInc.processEvent(event);
	return false;
}

bool StateTextSettings::update(float delta)
{
	m_tweenManager.update(delta);
	Hideable::update(delta);
	return false;
}

} // namespace NovelTea
