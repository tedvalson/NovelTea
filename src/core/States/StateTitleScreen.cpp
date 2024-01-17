#include <NovelTea/States/StateTitleScreen.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/ActiveTextSegment.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/TextInput.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateTitleScreen::StateTitleScreen(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_buttonStart(&context)
, m_buttonSettings(&context)
, m_buttonProfile(&context)
, m_textTitle(&context)
, m_textAuthor(&context)
, m_textProfile(&context)
, m_startPressed(false)
{
	auto bgColor = sf::Color(200, 200, 200);

	// Buttons
	m_buttonStart.setString("Start");
	m_buttonStart.setTextColor(sf::Color(80, 80, 80));
	m_buttonStart.setActiveColor(sf::Color(200, 200, 200, 240));
	m_buttonStart.setColor(sf::Color(220, 220, 220, 240));
	m_buttonStart.onClick([this, bgColor](){
		if (m_startPressed)
			return;
		m_startPressed = true;
		m_tweenManager.killAll();
		GSettings->save();
		TweenEngine::Tween::to(*this, ALPHA, 1.f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				GGame->reset();
				GGame->loadLast();
				requestStackClear();
				requestStackPush(StateID::Main);
			})
			.start(m_tweenManager);
		TweenEngine::Tween::to(m_bg, TweenRectangleShape::FILL_COLOR_RGB, 1.f)
			.target(bgColor.r, bgColor.g, bgColor.b)
			.start(m_tweenManager);
	});

	m_buttonSettings = m_buttonStart;
	m_buttonSettings.setString("Settings");
	m_buttonSettings.onClick([this](){
		requestStackPush(StateID::Settings);
	});

	m_buttonProfile = m_buttonStart;
	m_buttonProfile.setString("Profiles");
	m_buttonProfile.onClick([this](){
		requestStackPush(StateID::ProfileManager, false, [this](void*){
			updateProfileText();
			return true;
		});
	});

	setAlpha(0.f);
	TweenEngine::Tween::to(*this, ALPHA, 2.f)
		.target(255.f)
		.start(m_tweenManager);

	TweenEngine::Tween::to(m_bg, TweenRectangleShape::FILL_COLOR_RGB, 2.f)
		.target(bgColor.r, bgColor.g, bgColor.b)
		.start(m_tweenManager);
}

void StateTitleScreen::render(sf::RenderTarget &target)
{
	target.clear(m_bg.getFillColor());
	target.draw(m_textTitle);
	target.draw(m_textAuthor);
	target.draw(m_textProfile);
	target.draw(m_buttonSettings);
	target.draw(m_buttonProfile);
	target.draw(m_buttonStart);
}

void StateTitleScreen::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto portrait = (h > w);
	m_size = size;

	// Title
	TextProperties textProps;
	textProps.fontStyle |= sf::Text::Bold;
	textProps.fontSize = 0.1f * h;
	textProps.outlineThickness = 2.f;
	textProps.outlineColor = Color(200, 200, 200);
	m_textTitle.setSize(sf::Vector2f((portrait ? 0.95f : 0.8f) * w, h));
	m_textTitle.setFontSizeMultiplier(portrait ? 0.4f : 0.7f);
	m_textTitle.setText(ProjData[ID::projectName].ToString(), textProps);
	m_textTitle.setOrigin(m_textTitle.getLocalBounds().width / 2, 0.f);
	m_textTitle.setPosition(round(0.5f * w), round(0.05f * h));

	// Author
	textProps.fontStyle ^= sf::Text::Bold;
	textProps.fontSize = 0.03f * h;
	textProps.color = Color(120, 120, 120);
	textProps.outlineThickness = 1.f;
	m_textAuthor.setSize(sf::Vector2f(0.9f * w, h));
	m_textAuthor.setFontSizeMultiplier(portrait ? 0.35f : 0.6f);
	m_textAuthor.setPosition(round(0.5f * w - m_textTitle.getLocalBounds().width / 2),
							 round((0.07f * h) + m_textTitle.getLocalBounds().height));
	m_textAuthor.setText("created by " + ProjData[ID::projectAuthor].ToString(), textProps);

	auto buttonWidth = (portrait ? 0.85f : 0.4f) * w;
	auto buttonHeight = (portrait ? 0.09f : 0.12f) * h * 0.8f;
	m_buttonFontSize = buttonHeight * 0.7f;
	m_buttonStart.getText().setCharacterSize(m_buttonFontSize);
	m_buttonStart.setSize(buttonWidth, buttonHeight);
	m_buttonStart.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 4.2f));

	m_buttonSettings.getText().setCharacterSize(m_buttonFontSize);
	m_buttonSettings.setSize(buttonWidth, buttonHeight);
	m_buttonSettings.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 3.f));

	m_buttonProfile.getText().setCharacterSize(m_buttonFontSize);
	m_buttonProfile.setSize(buttonWidth, buttonHeight);
	m_buttonProfile.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 1.8f));

	updateProfileText();
}

void StateTitleScreen::setAlpha(float alpha)
{
	m_textTitle.setAlpha(alpha);
	m_textAuthor.setAlpha(alpha);
	m_textProfile.setAlpha(alpha);
	m_buttonStart.setAlpha(alpha);
	m_buttonSettings.setAlpha(alpha);
	m_buttonProfile.setAlpha(alpha);
	State::setAlpha(alpha);
}

void StateTitleScreen::updateProfileText()
{
	TextProperties textProps;
	textProps.color = Color(120, 120, 120);
	textProps.fontSize = 0.3f * m_buttonFontSize;
	if (GSettings->getProfiles().empty())
	{
		if (GTextInput.finished())
			GTextInput.run("Enter Profile Name", [this](const std::string &name){
				GSettings->addProfile(name);
				updateProfileText();
			});
		return;
	}
	m_textProfile.setText(GSettings->getActiveProfile()->getName(), textProps);
	auto b = m_textProfile.getGlobalBounds();
	m_textProfile.setPosition((m_size.x - b.width) / 2, m_size.y - b.height * 1.1f);
}

bool StateTitleScreen::processEvent(const sf::Event &event)
{
	m_buttonStart.processEvent(event);
	m_buttonSettings.processEvent(event);
	m_buttonProfile.processEvent(event);
	return true;
}

bool StateTitleScreen::update(float delta)
{
	m_textTitle.update(delta);
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
