#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/GUI/Notification.hpp>
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
, m_testRecordMode(false)
, m_roomTextChanging(false)
, m_cutsceneSpeed(1.f)
{
	ScriptMan.reset();

	auto width = getContext().config.width;
	auto height = getContext().config.height;
	m_roomActiveText.setSize(sf::Vector2f(width, 0.f));
	m_cutsceneRenderer.setSize(sf::Vector2f(width, 0.f));
	m_dialogueRenderer.setSize(sf::Vector2f(width, height));

	// Set all Navigation transforms before getGlobalBounds is called
	m_navigation.setScale(1.5f, 1.5f);
	m_navigation.setPosition(20.f, -20.f + height - m_navigation.getGlobalBounds().height);
	m_navigation.setCallback([this](const json &jentity){
		if (m_testRecordMode)
		{
			json jtestItem({
				"type", "room",
				"room", jentity[1].ToString()
			});
			runCallback(&jtestItem);
		}
		getContext().game.pushNextEntityJson(jentity);
	});

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

	m_actionBuilder.setPosition(10.f, -10.f + height - m_navigation.getGlobalBounds().height - 120.f);
	m_actionBuilder.setSize(sf::Vector2f(width - 20.f, 200.f));
	m_actionBuilder.setCallback([this](bool confirmed){
		if (confirmed)
		{
			processAction(m_actionBuilder.getVerb(), m_actionBuilder.getObjects());
		}
		m_actionBuilder.hide();
	});

	Notification::setScreenSize(sf::Vector2f(width, m_actionBuilder.getPosition().y - 4.f));

	m_textOverlay.setAlpha(0.f);
	m_textOverlay.setSize(sf::Vector2f(width, m_actionBuilder.getPosition().y));
	getContext().game.setMessageCallback([this](const std::vector<std::string> &messageArray, const DukValue &callback){
		m_textOverlayFunc = callback;
		if (!m_testPlaybackMode)
		{
			m_textOverlay.setTextArray(messageArray);
			m_textOverlay.show();
		}
		else
			callOverlayFunc();
	});

	auto &saveEntryPoint = GSave.data()[ID::entrypointEntity];
	auto &projEntryPoint = ProjData[ID::entrypointEntity];
	if (!saveEntryPoint.IsEmpty())
		setMode(saveEntryPoint);
	else if (!projEntryPoint.IsEmpty())
		setMode(projEntryPoint);

	if (getContext().data.hasKey("testSteps"))
	{
		m_testPlaybackMode = true;
		m_testRecordMode = getContext().data["record"].ToBool();
		if (m_testRecordMode)
			m_dialogueRenderer.setDialogueCallback([this](int index){
				json jtestItem({
					"type", "dialogue",
					"index", index
				});
				if (!m_testPlaybackMode)
					runCallback(&jtestItem);
			});
		processTestSteps();
	}
}

void StateMain::render(sf::RenderTarget &target)
{
	if (m_mode != Mode::Room && m_roomTextChanging)
		target.draw(m_roomActiveTextFadeOut);

	if (m_mode == Mode::Cutscene)
	{
		target.draw(m_cutsceneRenderer);
	}
	else if (m_mode == Mode::Dialogue)
	{
		target.draw(m_dialogueRenderer);
	}
	else if (m_mode == Mode::Room)
	{
		target.draw(m_roomActiveTextFadeOut);
		target.draw(m_roomActiveText);
		if (m_actionBuilder.isVisible())
			target.draw(m_actionBuilder);
		if (m_verbList.isVisible())
			target.draw(m_verbList);
	}

	target.draw(m_navigation);
	target.draw(m_textOverlay);

	for (auto &notification : Notification::notifications)
		target.draw(*notification);
}

void StateMain::setMode(Mode mode, const std::string &idName)
{
	m_roomActiveText.setHighlightId("");
	m_actionBuilder.hide();
	m_verbList.hide();

	if (mode != Mode::Room)
	{
		updateRoomText("");
		m_navigation.hide();
	}

	if (mode == Mode::Cutscene)
	{
		m_cutscene = GSave.get<Cutscene>(idName);
		m_cutsceneRenderer.setCutscene(m_cutscene);
	}
	else if (mode == Mode::Dialogue)
	{
		m_dialogue = GSave.get<Dialogue>(idName);
		m_dialogueRenderer.setDialogue(m_dialogue);
	}
	else if (mode == Mode::Room)
	{
		getContext().game.setRoomId(idName);
		updateRoomText();

		auto room = getContext().game.getRoom();
		m_navigation.setPaths(room->getPaths());
		m_navigation.show(1.f);
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
	else if (type == EntityType::Dialogue)
		mode = Mode::Dialogue;
	else if (type == EntityType::CustomScript)
	{
		mode = Mode::Nothing;
	}

	setMode(mode, idName);
}

void StateMain::processTestSteps()
{
	auto jsteps = getContext().data["testSteps"];
	for (int i = 0; i < jsteps.size(); ++i)
	{
		auto &jstep = jsteps[i];

		// Skip through all cutscenes
		while (m_mode == Mode::Cutscene)
		{
			update(0.001f * m_cutscene->getDelayMs());
		}

		auto type = jstep["type"].ToString();
		if (type == "action")
		{
			std::vector<std::string> objectIds;
			for (auto jobjectId : jstep["objects"].ArrayRange())
				objectIds.push_back(jobjectId.ToString());
			if (!processAction(jstep["verb"].ToString(), objectIds))
			{
				json j({
					"success", false,
					"index", i
				});
				runCallback(&j);
				return;
			}
		}
		else if (type == "dialogue")
		{
			m_dialogueRenderer.processSelection(jstep["index"].ToInt());
		}
		else if (type == "room")
		{
			getContext().game.pushNextEntity(GSave.get<Room>(jstep["room"].ToString()));
		}
		else if (type == "wait")
		{
			update(0.001f * jstep["duration"].ToInt());
		}

		if (m_mode != Mode::Cutscene)
			gotoNextEntity();
	}

	m_testPlaybackMode = false;
}

bool StateMain::processAction(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	auto action = Action::find(verbId, objectIds);
	auto success = true;
	auto verb = GSave.get<Verb>(verbId);

	for (auto &objectId : objectIds)
		if (!ActiveGame->getRoom()->containsId(objectId) && !ActiveGame->getObjectList()->containsId(objectId))
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
		// Don't record during playback to avoid dupliating all actions
		if (m_testRecordMode && !m_testPlaybackMode)
		{
			auto jobjects = sj::Array();
			for (auto &objectId : objectIds)
				jobjects.append(objectId);
			json jtestItem({
				"type", "action",
				"verb", verbId,
				"objects", jobjects
			});
			runCallback(&jtestItem);
		}
		updateRoomText();
	}
	return success;
}

void StateMain::gotoNextEntity()
{
	auto nextEntity = getContext().game.popNextEntity();
	if (!nextEntity)
	{
		if (m_mode != Mode::Room)
			nextEntity = getContext().game.getRoom();
		if (!nextEntity || nextEntity->getId().empty())
			return;
	}

	auto mode = Mode::Nothing;
	if (nextEntity->entityId() == Cutscene::id)
		mode = Mode::Cutscene;
	else if (nextEntity->entityId() == Room::id)
		mode = Mode::Room;
	else if (nextEntity->entityId() == Dialogue::id)
		mode = Mode::Dialogue;
	setMode(mode, nextEntity->getId());
}

void StateMain::updateRoomText(const std::string &newText)
{
	auto room = getContext().game.getRoom();
	auto text = newText;
	if (text == " ")
		text = ScriptMan.runInClosure<std::string>(room->getDescription());

	m_roomTextChanging = true;
	m_roomActiveTextFadeOut = m_roomActiveText;
	m_roomActiveText.setText(text);

	TweenEngine::Tween::from(m_roomActiveText, ActiveText::ALPHA, 0.5f)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_roomActiveTextFadeOut, ActiveText::ALPHA, 0.5f)
		.target(0.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
			m_roomTextChanging = false;
		}).start(m_tweenManager);
}

void StateMain::callOverlayFunc()
{
	if (m_textOverlayFunc.type() != DukValue::UNDEFINED){
		getContext().game.getScriptManager().call<void>(m_textOverlayFunc);
		updateRoomText();
	}
}

bool StateMain::processEvent(const sf::Event &event)
{
	if (m_textOverlay.isVisible())
	{
		if (m_textOverlay.processEvent(event))
			m_textOverlay.hide(0.5f, TextOverlay::ALPHA, [this](){
				callOverlayFunc();
			});
		return true;
	}

	if (m_roomTextChanging)
		return true;

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
	else if (m_mode == Mode::Dialogue)
	{
		m_dialogueRenderer.processEvent(event);
	}
	else if (m_mode == Mode::Room)
	{
		if (m_verbList.isVisible())
			if (!m_verbList.processEvent(event))
				return false;

		m_actionBuilder.processEvent(event);
		m_navigation.processEvent(event);

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
					m_verbList.setPositionBounded(p, sf::FloatRect(0.f, 0.f, getContext().config.width, getContext().config.height));
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
			getContext().game.pushNextEntityJson(m_cutscene->getNextEntity());
			gotoNextEntity();
		}
	}
	else if (m_mode == Mode::Dialogue)
	{
		m_dialogueRenderer.update(delta);
		if (m_dialogueRenderer.isComplete())
		{
			gotoNextEntity();
		}
	}
	else // if Mode::Nothing
		if (!m_roomTextChanging)
			gotoNextEntity();

	m_verbList.update(delta);
	m_actionBuilder.update(delta);
	m_navigation.update(delta);
	m_textOverlay.update(delta);

	Notification::update(delta);
	if (getContext().game.getTimerManager().update(delta))
		updateRoomText();

	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
