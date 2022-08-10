#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/PropertyList.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <iostream>

namespace NovelTea
{

Game::Game(Context* context)
	: ContextObject(context)
	, m_objectList(nullptr)
	, m_propertyList(nullptr)
	, m_map(nullptr)
	, m_room(nullptr)
	, m_autosaveEnabled(true)
	, m_quitting(false)
	, m_minimapEnabled(true)
	, m_navigationEnabled(true)
	, m_saveEnabled(true)
	, m_initialized(false)
	, m_messageCallback(nullptr)
	, m_saveCallback(nullptr)
	, m_settings(new Settings(context))
	, m_projectData(new ProjectData)
	, m_saveData(new SaveData)
{
}

Game::~Game()
{
	// TODO: Need to make sure active timers are destroyed first.
	//       shared_ptrs here may not clean up when expected?
	m_timerManager->reset();
}

// Game must be active before initialized
bool Game::initialize()
{
	if (m_initialized) {
		std::cerr << "Game already initialized!" << std::endl;
		return false;
	}

	m_saveData->setDirectory(GConfig.saveDir);

	m_settings->setDirectory(GConfig.settingsDir);
	m_settings->load();
	if (GConfig.projectData)
		m_projectData = GConfig.projectData;
	else
		m_projectData->loadFromFile(GConfig.projectFileName);

	reset();

	GSaveData[NovelTea::ID::entityPreview] = GConfig.entityPreview;
	GSaveData[NovelTea::ID::entrypointEntity] = GConfig.entryEntity;
	GSaveData[NovelTea::ID::entrypointMetadata] = GConfig.entryMeta;

	m_initialized = true;
	return true;
}

void Game::reset()
{
	if (!m_projectData->isLoaded())
		return;

	while (!m_entityQueue.empty())
		m_entityQueue.pop();

	m_quitting = false;
	m_saveData->setProfileIndex(m_settings->getActiveProfileIndex()); // Should reset() SaveData
	m_saveData->data()[ID::objectLocations][Room::id] = Room::getProjectRoomObjects(getContext());
	TimerMan->reset();
	ScriptMan->reset();
	syncToSave();
}

void Game::setRoomId(const std::string &roomId)
{
	m_room = get<Room>(roomId);
}

void Game::setRoom(const std::shared_ptr<Room> &room)
{
	m_room = room;
}

const std::shared_ptr<Room> &Game::getRoom() const
{
	if (m_room)
		m_room->sync();
	return m_room;
}

void Game::setMapId(const std::string &mapId)
{
	if (m_map && m_map->getId() == mapId)
		return;

	if (mapId.empty())
		m_map = nullptr;
	else
		m_map = get<Map>(mapId);
}

const std::string Game::getMapId() const
{
	if (!m_map)
		return "";
	return m_map->getId();
}

void Game::setMap(const std::shared_ptr<Map> &map)
{
	m_map = map;
}

const std::shared_ptr<Map> &Game::getMap() const
{
	return m_map;
}

DukValue Game::prop(const std::string &key, const DukValue &defaultValue)
{
	return m_propertyList->get(key, defaultValue);
}

void Game::setProp(const std::string &key, const DukValue &value)
{
	m_propertyList->set(key, value);
}

void Game::pushNextEntity(std::shared_ptr<Entity> entity, const DukValue &value)
{
	m_entityQueue.emplace(entity, value);
}

void Game::pushNextEntityJson(json jentity)
{
	pushNextEntity(Entity::fromEntityJson(getContext(), jentity));
}

std::shared_ptr<Entity> Game::popNextEntity()
{
	if (m_entityQueue.empty())
		return nullptr;
	auto nextPair = m_entityQueue.front();
	auto& nextEntity = nextPair.first;
	auto& callback = nextPair.second;
	m_entityQueue.pop();
	if (callback.type() != DukValue::UNDEFINED)
		ScriptMan->call<void>(callback);
	return nextEntity;
}

void Game::save(int slot)
{
	if (!getSaveEnabled())
		return;
	if (m_saveData->getProfileIndex() < 0)
		m_settings->ensureProfileExists();
	ScriptMan->runInClosure(ProjData[ID::scriptBeforeSave].ToString());
	if (m_saveCallback)
		m_saveCallback();
	m_saveData->data()[ID::mapEnabled] = m_minimapEnabled;
	m_saveData->data()[ID::navigationEnabled] = m_navigationEnabled;
	m_saveData->data()[ID::log] = GTextLog->toJson();
	m_saveData->save(slot);
}

bool Game::load(int slot)
{
	if (!m_saveData->load(slot))
		return false;
	syncToSave();
	ScriptMan->runInClosure(ProjData[ID::scriptAfterLoad].ToString());
	return true;
}

bool Game::loadLast()
{
	if (!m_saveData->loadLast())
		return false;
	syncToSave();
	ScriptMan->runInClosure(ProjData[ID::scriptAfterLoad].ToString());
	return true;
}

void Game::autosave()
{
	if (m_autosaveEnabled)
		save(0);
}

void Game::syncToSave()
{
	m_room = std::make_shared<Room>(getContext());
	m_objectList = std::make_shared<ObjectList>(getContext());
	m_propertyList = std::make_shared<PropertyList>(getContext());
	if (!m_saveData->isLoaded()) {
		auto &j = m_projectData->data()[ID::startingInventory];
		for (auto &jObjectId : j.ArrayRange())
			m_objectList->addId(jObjectId.ToString());
	}
	m_objectList->attach("player", "inv");
	m_propertyList->attach("game", "globals");
	GTextLog->fromJson(m_saveData->data()[ID::log]);
	m_minimapEnabled = m_saveData->data()[ID::mapEnabled].ToBool();
	m_navigationEnabled = m_saveData->data()[ID::navigationEnabled].ToBool();
}

void Game::quit()
{
	m_quitting = true;
}

bool Game::isQuitting() const
{
	return m_quitting;
}

void Game::spawnNotification(const std::string &message, bool addToLog, int durationMs)
{
	NotificationMan->spawn(message, durationMs);
	if (addToLog)
		GTextLog->push(message, TextLogType::Notification);
}

//void Game::execMessageCallback(const std::string &message, const DukValue &callback)
//{
//	m_messageCallback(message, callback);
//}

void Game::execMessageCallback(const std::vector<std::string> &messageArray, const DukValue &callback)
{
	if (m_messageCallback)
		m_messageCallback(messageArray, callback);
}

void Game::execMessageCallbackLog(const std::vector<std::string> &messageArray, const DukValue &callback)
{
	execMessageCallback(messageArray, callback);
	for (auto &s : messageArray)
		GTextLog->push(s, TextLogType::TextOverlay);
}

std::string Game::getParentId(const std::string &entityType, const std::string &entityId)
{
	if (entityType.empty())
		return std::string();

	json j;
	if (m_saveData->data()[entityType].hasKey(entityId))
		j = m_saveData->data()[entityType][entityId];
	else
		j = m_projectData->data()[entityType][entityId];
	return j[1].ToString();
}

void Game::set(std::shared_ptr<Entity> obj, const std::string &idName)
{
	m_saveData->set(obj, idName);
}

} // namespace NovelTea
