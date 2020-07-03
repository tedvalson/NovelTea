#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
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
	text.setPosition(0.f, 450.f);

	m_actionBuilder.setPosition(10.f, 500.f);
	m_actionBuilder.setSize(sf::Vector2f(getContext().config.width - 20.f, 200.f));

	m_verbList.setSelectCallback([this](const std::string &verbId){
		m_actionBuilder.setVerb(verbId);
		m_actionBuilder.setObject(m_selectedObjectId, 0);
		m_verbList.hide();
	});
	m_verbList.setShowHideCallback([this](bool showing){
		if (!showing)
		{
			activeText.setHighlightId("");
			m_actionBuilder.show();
		}
	});

	m_actionBuilder.setCallback([this](bool confirmed){
		if (confirmed)
		{
			auto action = m_actionBuilder.getAction();
			auto verb = Proj.get<Verb>(m_actionBuilder.getVerb());
			if (action)
			{
			}
			else if (!verb->getDefaultScriptSuccess().empty())
			{
			}
		}
		m_actionBuilder.hide();
	});
}

void StateEditor::render(sf::RenderTarget &target)
{
//	target.draw(text);
	if (mode == StateEditorMode::Cutscene)
		target.draw(cutsceneRenderer);
	else if (mode == StateEditorMode::Room)
	{
		target.draw(activeText);
		if (m_actionBuilder.isVisible())
			target.draw(m_actionBuilder);
		if (m_verbList.isVisible())
			target.draw(m_verbList);
	}

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
			std::string data = jsonData["data"];
			auto r = ScriptMan.runInClosure<std::string>(data);

			activeText.setText(r);
			activeText.setPosition(10.f, 10.f);
			activeText.setSize(sf::Vector2f(460.f, 600.f));
			activeText.setCursorStart(sf::Vector2f(50.f, 25.f));
		}
	}

	(*resp)["test"] = "pass";

	return resp;
}

bool StateEditor::processEvent(const sf::Event &event)
{
	if (m_verbList.isVisible())
		if (!m_verbList.processEvent(event))
			return false;

	m_actionBuilder.processEvent(event);

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (mode == StateEditorMode::Room)
		{
			if (!m_verbList.isVisible())
			{
				auto p = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
				auto word = activeText.objectFromPoint(p);
				activeText.setHighlightId(word);
				if (!word.empty())
				{
					m_selectedObjectId = word;
					m_verbList.setVerbs(word);
					m_verbList.setPosition(p);
					m_verbList.show();
				}
			}
			else if (!m_verbList.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
			{
				m_verbList.hide();
			}
		}
	}
	if (event.type == sf::Event::MouseButtonPressed)
	{

	}

	return true;
}

bool StateEditor::update(float deltaSeconds)
{
//	cutsceneRenderer.update(deltaSeconds);
	m_verbList.update(deltaSeconds);
	m_actionBuilder.update(deltaSeconds);
	tweenManager.update(deltaSeconds);
	return true;
}

} // namespace NovelTea
