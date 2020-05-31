#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Cutscene.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateEditor::StateEditor(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, mode(StateEditorMode::Nothing)
{
	font.loadFromFile("/home/android/dev/NovelTea/res/fonts/DejaVuSans.ttf");

	text.setFont(font);
	text.setCharacterSize(30);
	text.setString("Testing test!?");
	text.setFillColor(sf::Color::Black);
	text.setOutlineColor(sf::Color::Yellow);
	text.setOutlineThickness(1.f);
	text.setPosition(0.f, 250.f);
}

void StateEditor::render(sf::RenderTarget &target)
{
//	target.draw(text);
	if (mode == StateEditorMode::Cutscene)
		target.draw(cutsceneRenderer);
	else if (mode == StateEditorMode::Room)
		target.draw(activeText);
}

void *StateEditor::processData(void *data)
{
	auto &jsonData = *static_cast<json*>(data);
	auto resp = new json;
	auto event = jsonData["event"];

	if (event == "mode")
	{
		mode = jsonData["mode"];
	}
	else if (mode == StateEditorMode::Cutscene)
	{
		if (event == "cutscene")
		{
			auto cutscene = std::make_shared<Cutscene>(jsonData["cutscene"]);
			cutsceneRenderer.setCutscene(cutscene);
			cutsceneRenderer.setPosition(10.f, 10.f);
		}
		else if (event == "setPlaybackTime")
		{
			int ms = jsonData["value"];
			cutsceneRenderer.reset();
			cutsceneRenderer.update(0.001f * ms);
		}
		else if (event == "update")
		{
			size_t deltaMs = jsonData["delta"];
			auto delta = 0.001f * deltaMs;
			cutsceneRenderer.update(delta);
		}
	}
	else if (mode == StateEditorMode::Room)
	{
		if (event == "text")
		{
			activeText.setText(jsonData["data"]);
			activeText.setPosition(10.f, 10.f);
			activeText.setSize(sf::Vector2f(300.f, 500.f));
			activeText.setCursorStart(sf::Vector2f(50.f, 25.f));
		}
	}

	(*resp)["test"] = "pass";

	return resp;
}

bool StateEditor::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonReleased)
	{

	}

	return true;
}

bool StateEditor::update(float deltaSeconds)
{
//	cutsceneRenderer.update(deltaSeconds);
	tweenManager.update(deltaSeconds);
	return true;
}

} // namespace NovelTea
