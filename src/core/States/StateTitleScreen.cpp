#include <NovelTea/States/StateTitleScreen.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
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
, m_startPressed(false)
{
	auto bgColor = context.config.backgroundColor;

	// Buttons
	m_buttonStart.setString("Start");
	m_buttonStart.setTextColor(sf::Color(80, 80, 80));
	m_buttonStart.setActiveColor(sf::Color(0, 0, 0, 50));
	m_buttonStart.setColor(sf::Color(0, 0, 0, 30));
	m_buttonStart.onClick([this, bgColor](){
		if (m_startPressed)
			return;
		m_startPressed = true;
		m_tweenManager.killAll();
		GSettings.save();
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
	TextFormat format;
	format.size(0.1f * h);
	format.bold(true);
	m_textTitle.setSize(sf::Vector2f((portrait ? 0.95f : 0.8f) * w, h));
	m_textTitle.setFontSizeMultiplier(portrait ? 0.4f : 0.7f);
	m_textTitle.setText(ProjData[ID::projectName].ToString(), format);
	m_textTitle.setOrigin(m_textTitle.getLocalBounds().width / 2, 0.f);
	m_textTitle.setPosition(round(0.5f * w), round(0.05f * h));

	// Author
	format.color(sf::Color(120, 120, 120));
	format.size(0.03f * h);
	format.bold(false);
	m_textAuthor.setSize(sf::Vector2f(0.9f * w, h));
	m_textAuthor.setFontSizeMultiplier(portrait ? 0.35f : 0.6f);
	m_textAuthor.setPosition(round(0.5f * w - m_textTitle.getLocalBounds().width / 2),
							 round((0.07f * h) + m_textTitle.getLocalBounds().height));
	m_textAuthor.setText("created by " + ProjData[ID::projectAuthor].ToString(), format);

	auto buttonWidth = (portrait ? 0.85f : 0.4f) * w;
	auto buttonHeight = (portrait ? 0.09f : 0.12f) * h * 0.8f;
	auto buttonFontSize = buttonHeight * 0.7f;
	m_buttonStart.getText().setCharacterSize(buttonFontSize);
	m_buttonStart.setSize(buttonWidth, buttonHeight);
	m_buttonStart.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 4.2f));

	m_buttonSettings.getText().setCharacterSize(buttonFontSize);
	m_buttonSettings.setSize(buttonWidth, buttonHeight);
	m_buttonSettings.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 3.f));

	m_buttonProfile.getText().setCharacterSize(buttonFontSize);
	m_buttonProfile.setSize(buttonWidth, buttonHeight);
	m_buttonProfile.setPosition(round(0.5f * (w - buttonWidth)), round(h - buttonHeight * 1.8f));

	m_formatProfile.color(sf::Color(120, 120, 120));
	m_formatProfile.size(0.3f * buttonFontSize);
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
	if (GSettings.getProfiles().empty())
	{
		if (GTextInput.finished())
			GTextInput.run("Enter Profile Name", [this](const std::string &name){
				GSettings.addProfile(name);
				updateProfileText();
			});
		return;
	}
	m_textProfile.setText(GSettings.getActiveProfile()->getName(), m_formatProfile);
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
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
