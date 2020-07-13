#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/Player.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateMain::StateMain(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_mode(Mode::Nothing)
, m_cutsceneSpeed(1.f)
{
	ScriptMan.reset();
	font.loadFromFile("/home/android/dev/NovelTea/res/fonts/DejaVuSans.ttf");

	text.setFont(font);
	text.setCharacterSize(30);
	text.setString("Hello World!");
	text.setFillColor(sf::Color::Black);
	text.setOutlineColor(sf::Color::Yellow);
	text.setOutlineThickness(1.f);
	text.setPosition(0.f, 250.f);

	m_cutsceneRenderer.setModeCallback([this](const json& jEntity){
		setMode(jEntity);
	});

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
				auto object = Save.get<Object>(m_selectedObjectId);
				auto s = "function f(object1,object2,object3,object4){"+action->getScript()+"}";
				ScriptMan.call(s, "f", object);
			}
			else if (!verb->getDefaultScriptSuccess().empty())
			{
				auto object = Save.get<Object>(m_selectedObjectId);
				auto s = "function f(object1,object2,object3,object4){"+verb->getDefaultScriptSuccess()+"}";
				ScriptMan.call(s, "f", object);
			}
			else
				std::cout << "Nothing happened." << std::endl;

			updateRoomText();
		}
		m_actionBuilder.hide();
	});

	// TODO: check SaveData for last entrypoint
	auto &entryPoint = ProjData[ID::projectEntrypoint];
	setMode(entryPoint);
}

void StateMain::render(sf::RenderTarget &target)
{
	if (m_mode == Mode::Cutscene)
	{
		target.draw(m_cutsceneRenderer);
	}
	else if (m_mode == Mode::Room)
	{
		target.draw(m_roomActiveText);
		if (m_actionBuilder.isVisible())
			target.draw(m_actionBuilder);
		if (m_verbList.isVisible())
			target.draw(m_verbList);
	}
}

void StateMain::setMode(Mode mode, const std::string &idName)
{
	if (mode == Mode::Cutscene)
	{
		auto cutscene = Save.get<Cutscene>(idName);
		m_cutsceneRenderer.setCutscene(cutscene);
	}
	else if (mode == Mode::Room)
	{
		Player::instance().setRoomId(idName);
		updateRoomText();
	}

	m_mode = mode;
}

void StateMain::setMode(const json &jEntity)
{
	auto mode = Mode::Nothing;
	auto type = jEntity[ID::entityType];
	auto idName = jEntity[ID::entityId];

	if (type == EntityType::Cutscene)
		mode = Mode::Cutscene;
	else if (type == EntityType::Room)
		mode = Mode::Room;
	else if (type == EntityType::CustomScript)
	{
		mode = Mode::Nothing;
	}

	setMode(mode, idName);
}

void StateMain::updateRoomText()
{
	auto room = Player::instance().getRoom();
	auto r = ScriptMan.runInClosure<std::string>(room->getDescription());
	m_roomActiveText.setText(r);
}

bool StateMain::processEvent(const sf::Event &event)
{
	if (m_mode == Mode::Cutscene)
	{
		if (event.type == sf::Event::MouseButtonReleased)
		{
			m_cutsceneSpeed = 1.f;
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			m_cutsceneSpeed = 10.f;
		}
	}
	else if (m_mode == Mode::Room)
	{
		if (m_verbList.isVisible())
			if (!m_verbList.processEvent(event))
				return false;

		m_actionBuilder.processEvent(event);

		if (event.type == sf::Event::MouseButtonReleased)
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

	return true;
}

bool StateMain::update(float delta)
{
	if (m_mode == Mode::Cutscene)
	{
		m_cutsceneRenderer.update(delta * m_cutsceneSpeed);
	}

	m_verbList.update(delta);
	m_actionBuilder.update(delta);

	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
