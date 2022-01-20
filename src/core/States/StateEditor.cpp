#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Verb.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateEditor::StateEditor(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_scrollPos(0.f)
, m_mode(StateEditorMode::Nothing)
{
	ScriptMan.reset();

	m_cutsceneRenderer.setSkipWaitingForClick(true);
	m_cutsceneRenderer.setSkipScriptSegments(true);
	m_cutsceneRenderer.setSkipConditionChecks(true);

	m_roomScrollbar.setColor(sf::Color(0, 0, 0, 40));
	m_roomScrollbar.setAutoHide(false);
	m_roomScrollbar.attachObject(this);
}

void StateEditor::render(sf::RenderTarget &target)
{
//	target.draw(text);
	if (m_mode == StateEditorMode::Cutscene)
		target.draw(m_cutsceneRenderer);
	else if (m_mode == StateEditorMode::Room)
	{
		target.draw(m_roomActiveText);
		target.draw(m_roomScrollbar);
	}
}

void StateEditor::resize(const sf::Vector2f &size)
{
	m_roomTextPadding = round(1.f / 16.f * std::min(size.x, size.y));

	m_roomActiveText.setSize(sf::Vector2f((size.x < size.y ? 1.f : 0.6f) * size.x - m_roomTextPadding*2, 0.f));

	m_cutsceneRenderer.setMargin(m_roomTextPadding);
	m_cutsceneRenderer.setSize(size);

	m_roomScrollbar.setPosition(size.x - 4.f, 4.f);
	m_roomScrollbar.setSize(sf::Vector2f(2, size.y - 8.f));
	m_roomScrollbar.setScrollAreaSize(size);
	m_roomScrollbar.setDragRect(sf::FloatRect(0.f, 0.f, size.x, size.y));

	m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
	updateScrollbar();
	m_roomScrollbar.setScroll(0.f);
}

void StateEditor::repositionText()
{
	auto w = getContext().config.width;
	m_roomActiveText.setPosition((w - m_roomActiveText.getSize().x)/2, m_roomTextPadding + m_scrollPos);
}

void StateEditor::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionText();
}

float StateEditor::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &StateEditor::getScrollSize()
{
	return m_scrollAreaSize;
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
	else if (event == "test")
	{
		// TODO: Need safer callback passing
		auto strCallbackPtr = jsonData["callback"].ToString();
		auto intCallbackPtr = strtoll(strCallbackPtr.c_str(), nullptr, 16);
		auto callback = *reinterpret_cast<TestCallback*>(intCallbackPtr);

		StateCallback stateCallback = [callback](void* data){
			auto j = *reinterpret_cast<json*>(data);
			return callback(j);
		};

		getContext().data["test"] = jsonData["test"];
		getContext().data["record"] = jsonData["record"];
		getContext().data["stopIndex"] = jsonData["stopIndex"];

		requestStackPop();
		requestStackPush(StateID::Main, false, stateCallback);
	}
	else if (m_mode == StateEditorMode::Cutscene)
	{
		if (event == "cutscene")
		{
			auto cutscene = std::make_shared<Cutscene>();
			cutscene->fromJson(jsonData["cutscene"]);
			m_cutsceneRenderer.setCutscene(cutscene);
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
			try {
				ScriptMan.reset();
				auto r = ActiveGame->getRoom()->getDescription();
				m_roomActiveText.setText(r);
			} catch (std::exception &e) {
				m_roomActiveText.setText(e.what());
			}

			m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
			updateScrollbar();
			m_roomScrollbar.setScroll(m_scrollPos);
		}
	}

	(*resp)["test"] = "pass";

	return resp;
}

bool StateEditor::processEvent(const sf::Event &event)
{
	if (m_mode == StateEditorMode::Room)
	{
		if (m_roomScrollbar.processEvent(event))
			return true;
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
	}
	if (event.type == sf::Event::MouseButtonPressed)
	{

	}

	return true;
}

bool StateEditor::update(float delta)
{
	m_roomScrollbar.update(delta);
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
