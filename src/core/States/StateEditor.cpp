#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/SaveData.hpp>
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
, m_mode(StateEditorMode::Nothing)
{
	ScriptMan.reset();

	text.setFont(*Proj.getFont(0));
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
			m_roomActiveText.setHighlightId("");
			if (!m_selectedObjectId.empty())
				m_actionBuilder.show();
		}
	});

	m_actionBuilder.setCallback([this](bool confirmed){
		if (confirmed)
		{
			auto action = m_actionBuilder.getAction();
			auto verb = Save.get<Verb>(m_actionBuilder.getVerb());
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
	if (m_mode == StateEditorMode::Cutscene)
		target.draw(m_cutsceneRenderer);
	else if (m_mode == StateEditorMode::Room)
	{
		target.draw(m_roomActiveText);
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
	auto event = jsonData["event"].ToString();

	if (event == "mode")
	{
		m_mode = static_cast<StateEditorMode>(jsonData["mode"].ToInt());
	}
	else if (m_mode == StateEditorMode::Cutscene)
	{
		if (event == "cutscene")
		{
			auto cutscene = std::make_shared<Cutscene>();
			cutscene->fromJson(jsonData["cutscene"]);
			m_cutsceneRenderer.setCutscene(cutscene);
			m_cutsceneRenderer.setPosition(10.f, 10.f);
		}
		else if (event == "setPlaybackTime")
		{
			auto ms = jsonData["value"].ToInt();
			m_cutsceneRenderer.reset();
			m_cutsceneRenderer.update(0.001f * ms);
		}
		else if (event == "update")
		{
			auto deltaMs = jsonData["delta"].ToInt();
			auto delta = 0.001f * deltaMs;
			m_cutsceneRenderer.update(delta);
		}
	}
	else if (m_mode == StateEditorMode::Room)
	{
		if (event == "text")
		{
			auto data = jsonData["data"].ToString();
			ScriptMan.reset();
			auto r = ScriptMan.runInClosure<std::string>(data);

			m_roomActiveText.setText(r);
			m_roomActiveText.setPosition(10.f, 10.f);
			m_roomActiveText.setSize(sf::Vector2f(460.f, 600.f));
			m_roomActiveText.setCursorStart(sf::Vector2f(50.f, 25.f));
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
		if (m_mode == StateEditorMode::Room)
		{
			if (!m_verbList.isVisible())
			{
				auto p = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
				auto word = m_roomActiveText.objectFromPoint(p);
				m_roomActiveText.setHighlightId(word);
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
				m_selectedObjectId = "";
				m_verbList.hide();
			}
		}
	}
	if (event.type == sf::Event::MouseButtonPressed)
	{

	}

	return true;
}

bool StateEditor::update(float delta)
{
	m_verbList.update(delta);
	m_actionBuilder.update(delta);
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
