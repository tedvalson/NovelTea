#include <NovelTea/StateEventManager.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutscenePlayer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialoguePlayer.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/Err.hpp>

namespace NovelTea {

StateEventManager::StateEventManager(Context *context)
: Subsystem(context)
, m_mode(ModeNothing)
, m_testPlaybackMode(false)
, m_testRecordMode(false)
, m_quitting(false)
, m_playTime(0)
, m_cutscene(nullptr)
, m_cutsceneSpeed(1.f)
, m_dialogue(nullptr)
{
}

StateEventManager::~StateEventManager()
{
}

bool StateEventManager::initialize()
{
	GGame->reset();
	GGame->loadLast();

	m_cutscenePlayer = std::make_shared<CutscenePlayer>(getContext());
	m_dialoguePlayer = std::make_shared<DialoguePlayer>(getContext());

	EventMan->listen([this](const EventPtr& event){
		auto type = event->type();
		if (type == Event::TimerCompleted) {
//			updateUI();
		}
		else if (type == StateEvent::CutsceneContinue) {
			if (m_mode != ModeCutscene)
				return false;
			m_cutscenePlayer->click();
		}
		else if (type == StateEvent::DialogueChoice) {
			auto choice = event->number;
			return m_dialoguePlayer->processSelection(choice);
		}
		else if (type == StateEvent::DialogueContinue) {
			if (m_mode != ModeDialogue)
				return false;
			return m_dialoguePlayer->continueToNext();
		}
		else if (type == StateEvent::MessageContinue) {
			callOverlayFunc();
		}

		return true;
	});

	ScriptMan->reset();
	GGame->setMessageCallback([this](const std::vector<std::string> &messageArray, const DukValue &callback){
		m_textOverlayFunc = callback;
		if (!m_testPlaybackMode)
			EventMan->push(StateEvent::MessageEvent{messageArray});
		else
			callOverlayFunc();
	});

	GGame->setSaveCallback([this](){
		auto entityType = EntityType::Room;
		auto entityId = GGame->getRoom()->getId();
		auto metaData = sj::Array(entityId);

		if (m_mode == ModeCutscene) {
			entityType = EntityType::Cutscene;
			entityId = m_cutscenePlayer->getCutscene()->getId();
			metaData.append(m_cutscenePlayer->saveState());
		} else if (m_mode == ModeDialogue) {
			entityType = EntityType::Dialogue;
			entityId = m_dialoguePlayer->getDialogue()->getId();
			metaData.append(m_dialoguePlayer->saveState());
		}

		GSaveData[ID::entrypointEntity] = sj::Array(
			static_cast<int>(entityType),
			entityId
		);
		auto& map = GGame->getMap();
		GSaveData[ID::entrypointMetadata] = metaData;
		GSaveData[ID::playTime] = m_playTime;
		GSaveData[ID::map] = map ? map->getId() : "";
		std::cout << "saving..." << std::endl;
	});

	m_playTime = GSaveData[ID::playTime].ToFloat();

	auto &saveEntryPoint = GSaveData[ID::entrypointEntity];
	auto &projEntryPoint = ProjData[ID::entrypointEntity];
	auto &entryMetadata = GSaveData[ID::entrypointMetadata];
	if (!saveEntryPoint.IsEmpty())
	{
		std::cout << "save entry point: " << saveEntryPoint << std::endl;
		std::cout << "save entry meta: " << entryMetadata << std::endl;
		auto roomId = entryMetadata[0].ToString();
		std::cout << "roomId: " << roomId << std::endl;
		auto mapId = GSaveData[ID::map].ToString();
		auto entryPointType = static_cast<EntityType>(saveEntryPoint[0].ToInt());
		GGame->setMapId(mapId);
//		m_mapRenderer.setActiveRoomId(roomId);
//		m_mapRenderer.setMap(GGame->getMap());

		if (!roomId.empty())
		{
			GGame->pushNextEntity(GGame->get<Room>(roomId));
			gotoNextEntity();
		}
		// Don't double-push room mode
		if (entryPointType != EntityType::Room)
		{
			GGame->pushNextEntityJson(saveEntryPoint);
			if (entryMetadata.size() > 1 && gotoNextEntity()) {
				if (entryPointType == EntityType::Cutscene)
					m_cutscenePlayer->restoreState(entryMetadata[1]);
				else if (entryPointType == EntityType::Dialogue)
					m_dialoguePlayer->restoreState(entryMetadata[1]);
			}
		}
	}
	else if (!projEntryPoint.IsEmpty())
		GGame->pushNextEntityJson(projEntryPoint);
	else
		err() << "No entry point?" << std::endl;

//	processTest();
	std::cout << "StateEventManager inititalized" << std::endl;
	return true;
}

void StateEventManager::update(float delta)
{
	auto& map = GGame->getMap();
//	auto& oldMap = m_mapRenderer.getMap();
//	if (map && (!oldMap || oldMap->getId() != map->getId()))
//		m_mapRenderer.setMap(map);

	m_playTime += delta;

	if (m_mode == ModeCutscene)
	{
		m_cutscenePlayer->update(delta * m_cutsceneSpeed);
		if (m_cutscenePlayer->isComplete())
		{
			GGame->pushNextEntityJson(m_cutscene->getNextEntityJson());
			gotoNextEntity();
		}
	}
	else if (m_mode == ModeDialogue)
	{
		m_dialoguePlayer->update(delta);
		if (m_dialoguePlayer->isComplete())
		{
			std::cout << "Next: " << m_dialogue->getNextEntityJson() << std::endl;
			GGame->pushNextEntityJson(m_dialogue->getNextEntityJson());
			gotoNextEntity();
		}
	}
	else // if Mode::Nothing
		gotoNextEntity();
}

void StateEventManager::setMode(EntityMode mode, const std::string &idName)
{
	std::cout << "---------- setMode: " << (int)mode << " \"" << idName << "\"" << std::endl;

	if (mode == ModeCutscene)
	{
		m_cutscene = GGame->get<Cutscene>(idName);
		m_cutscenePlayer->setCutscene(m_cutscene);
	}
	else if (mode == ModeDialogue)
	{
		m_dialogue = GGame->get<Dialogue>(idName);
		m_dialoguePlayer->setDialogue(m_dialogue);
	}
	else if (mode == ModeRoom)
	{
		auto nextRoom = GGame->get<Room>(idName);
		auto room = GGame->getRoom();
		if (room->getId() != idName)
		{
			if (room->getId().empty()) {
				GGame->setRoom(nextRoom);
				if (GSaveData[NovelTea::ID::entityPreview].ToBool())
					nextRoom->runScriptAfterEnter();
			} else {
				if (!room->runScriptBeforeLeave() || !nextRoom->runScriptBeforeEnter()) {
					updateRoomText();
					return;
				}
				room->setVisitCount(room->getVisitCount() + 1);
				room->runScriptAfterLeave();
				GGame->setRoom(nextRoom);
				nextRoom->runScriptAfterEnter();
			}
			// Should be set after Game room is changed, for map script eval purposes
//			m_mapRenderer.setActiveRoomId(nextRoom->getId());
		}
//		updateUI();
//		m_navigation.setPaths(nextRoom->getPaths());
	}

	EventMan->push({StateEvent::ModeChanged, mode});

	m_mode = mode;

	if (mode == ModeRoom)
		updateRoomText();
}

void StateEventManager::setMode(const sj::JSON &jEntity)
{
	auto mode = ModeNothing;
	auto type = static_cast<EntityType>(jEntity[ID::selectEntityType].ToInt());
	auto idName = jEntity[ID::selectEntityId].ToString();

	if (type == EntityType::Cutscene)
		mode = ModeCutscene;
	else if (type == EntityType::Room)
		mode = ModeRoom;
	else if (type == EntityType::Dialogue)
		mode = ModeDialogue;
	else if (type == EntityType::Script)
		ScriptMan->runScriptId(idName);
	else if (type == EntityType::CustomScript)
		ScriptMan->runInClosure(idName);

	if (type == EntityType::Script || type == EntityType::CustomScript) {
		mode = ModeRoom;
		idName = GGame->getRoom()->getId();
	}

	setMode(mode, idName);
}

bool StateEventManager::gotoNextEntity()
{
	if (m_quitting)
		return false;

	auto nextEntity = GGame->popNextEntity();
	if (!nextEntity)
	{
		if (m_mode != ModeRoom)
			nextEntity = GGame->getRoom();
		if (!nextEntity || nextEntity->getId().empty())
			return false;
	}

	auto mode = ModeNothing;
	if (nextEntity->entityId() == Action::id) {
		auto action = std::static_pointer_cast<Action>(nextEntity);
		action->runScript();
		return true;
	} else if (nextEntity->entityId() == Cutscene::id)
		mode = ModeCutscene;
	else if (nextEntity->entityId() == Room::id)
		mode = ModeRoom;
	else if (nextEntity->entityId() == Dialogue::id)
		mode = ModeDialogue;
	else if (nextEntity->entityId() == Script::id) {
		auto roomId = GGame->getRoom()->getId();
		auto script = std::static_pointer_cast<Script>(nextEntity);
		setMode(ModeRoom, roomId);
		ScriptMan->runScript(script);
		return true;
	}

	setMode(mode, nextEntity->getId());
	return true;
}

void StateEventManager::updateRoomText()
{
	auto room = GGame->getRoom();
	auto firstVisit = !GSaveData[ID::roomDescriptions].hasKey(room->getId());
	auto text = room->getDescription();
	if (text == m_roomText)
		return;
	m_roomText = text;

	EventMan->push({StateEvent::RoomTextChanged, text});
}

void StateEventManager::move(int direction, const sj::JSON &jentity)
{
	if (m_testRecordMode) {
		json jtestItem({
			"type", "move",
			"direction", direction
		});
//		runCallback(&jtestItem);
	}
	GGame->pushNextEntityJson(jentity);
}

void StateEventManager::callOverlayFunc()
{
	if (m_textOverlayFunc.type() != DukValue::UNDEFINED){
		ScriptMan->call<void>(m_textOverlayFunc);
//		updateUI();
	}
}

} // namespace NovelTea
