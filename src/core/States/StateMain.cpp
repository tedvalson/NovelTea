#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateMain::StateMain(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_mode(Mode::Nothing)
, m_testPlaybackMode(false)
, m_cutsceneSpeed(1.f)
{
	ScriptMan.reset();

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
			processAction(m_actionBuilder.getVerb(), m_actionBuilder.getObjects());
		}
		m_actionBuilder.hide();
	});

	auto &saveEntryPoint = GSave.data()[ID::entrypointEntity];
	auto &projEntryPoint = ProjData[ID::entrypointEntity];
	if (!saveEntryPoint.IsEmpty())
		setMode(saveEntryPoint);
	else if (!projEntryPoint.IsEmpty())
		setMode(projEntryPoint);
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
		m_cutscene = GSave.get<Cutscene>(idName);
		m_cutsceneRenderer.setCutscene(m_cutscene);
		getContext().game.pushNextEntityJson(m_cutscene->getNextEntity());
	}
	else if (mode == Mode::Room)
	{
		getContext().game.setRoomId(idName);
		updateRoomText();
	}

	m_mode = mode;
}

void StateMain::setMode(const json &jEntity)
{
	auto mode = Mode::Nothing;
	auto type = static_cast<EntityType>(jEntity[ID::selectEntityType].ToInt());
	auto idName = jEntity[ID::selectEntityId].ToString();

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

bool StateMain::processAction(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	auto action = Action::find(verbId, objectIds);
	auto success = true;
	auto verb = GSave.get<Verb>(verbId);

	for (auto &objectId : objectIds)
		if (!ActiveGame->getRoom()->contains(objectId) && !ActiveGame->getObjectList()->contains(objectId))
			return false;

	if (action)
	{
		auto object = GSave.get<Object>(objectIds[0]);
		auto s = "function f(object1,object2,object3,object4){"+action->getScript()+"}";
		ScriptMan.call(s, "f", object);
	}
	else if (!verb->getDefaultScriptSuccess().empty())
	{
		auto object = GSave.get<Object>(objectIds[0]);
		auto s = "function f(object1,object2,object3,object4){"+verb->getDefaultScriptSuccess()+"}";
		ScriptMan.call(s, "f", object);
	}
	else
	{
		success = false;
		std::cout << "Nothing happened." << std::endl;
	}

	if (success)
	{
		updateRoomText();
	}
	return success;
}

void StateMain::gotoNextEntity()
{
	auto nextEntity = getContext().game.popNextEntity();
	if (nextEntity)
	{
		auto mode = Mode::Nothing;
		if (nextEntity->entityId() == Cutscene::id)
			mode = Mode::Cutscene;
		else if (nextEntity->entityId() == Room::id)
			mode = Mode::Room;
		else if (nextEntity->entityId() == Dialogue::id)
			mode = Mode::Dialogue;
		setMode(mode, nextEntity->getId());
	}
}

void StateMain::updateRoomText()
{
	auto room = getContext().game.getRoom();
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
		if (m_cutsceneRenderer.isComplete())
		{
			gotoNextEntity();
		}
	}
	else // if Mode::Nothing
		gotoNextEntity();

	m_verbList.update(delta);
	m_actionBuilder.update(delta);

	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
