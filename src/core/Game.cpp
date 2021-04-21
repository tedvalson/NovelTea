#include <NovelTea/Game.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <iostream>

namespace NovelTea
{

Game::Game()
	: m_objectList(nullptr)
	, m_propertyList(nullptr)
	, m_room(nullptr)
	, m_autosaveEnabled(true)
	, m_quitting(false)
	, m_messageCallback(nullptr)
	, m_saveCallback(nullptr)
	, m_saveData(new SaveData)
	, m_timerManager(new TimerManager)
	, m_scriptManager(new ScriptManager(this))
	, m_notificationManager(new NotificationManager)
	, m_textLog(new TextLog)
{
}

Game::~Game()
{
	// TODO: Need to make sure active timers are destroyed first.
	//       shared_ptrs here may not clean up when expected?
	m_timerManager->reset();
}

// Game must be active before initialized
void Game::initialize()
{
	reset();
}

void Game::reset()
{
	if (!Proj.isLoaded())
		return;

	while (!m_entityQueue.empty())
		m_entityQueue.pop();

	m_quitting = false;
	m_saveData->reset();
	m_timerManager->reset();
	m_scriptManager->reset();
	syncToSave();
}

void Game::setRoomId(const std::string &roomId)
{
	m_roomId = roomId;
	m_room = m_saveData->get<Room>(roomId);
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

DukValue Game::prop(const std::string &key, const DukValue &defaultValue)
{
	return m_propertyList->get(key, defaultValue);
}

void Game::setProp(const std::string &key, const DukValue &value)
{
	m_propertyList->set(key, value);
}

void Game::pushNextEntity(std::shared_ptr<Entity> entity)
{
	m_entityQueue.push(entity);
}

void Game::pushNextEntityJson(json jentity)
{
	auto type = static_cast<EntityType>(jentity[ID::selectEntityType].ToInt());
	auto idName = jentity[ID::selectEntityId].ToString();
	if (type == EntityType::Action)
		pushNextEntity(m_saveData->get<Action>(idName));
	else if (type == EntityType::Cutscene)
		pushNextEntity(m_saveData->get<Cutscene>(idName));
	else if (type == EntityType::Room)
		pushNextEntity(m_saveData->get<Room>(idName));
	else if (type == EntityType::Dialogue)
		pushNextEntity(m_saveData->get<Dialogue>(idName));
	else if (type == EntityType::Script)
		pushNextEntity(m_saveData->get<Script>(idName));
	else if (type == EntityType::CustomScript) {
		auto script = std::make_shared<Script>();
		script->setContent(idName);
		pushNextEntity(script);
	}
}

std::shared_ptr<Entity> Game::popNextEntity()
{
	if (m_entityQueue.empty())
		return nullptr;
	auto nextEntity = m_entityQueue.front();
	m_entityQueue.pop();
	return nextEntity;
}

void Game::save(int slot)
{
	if (m_saveCallback)
		m_saveCallback();
	m_saveData->save(slot);
}

bool Game::load(int slot)
{
	if (!m_saveData->load(slot))
		return false;
	syncToSave();
	return true;
}

bool Game::loadLast()
{
	auto success = m_saveData->loadLast();
	if (success)
		syncToSave();
	return success;
}

void Game::autosave()
{
	if (m_autosaveEnabled)
		save(0);
}

void Game::syncToSave()
{
	m_room = std::make_shared<Room>();
	m_objectList = std::make_shared<ObjectList>(m_saveData);
	m_propertyList = std::make_shared<PropertyList>();
	if (!m_saveData->isLoaded()) {
		auto &j = ProjData[ID::startingInventory];
		for (auto &jObjectId : j.ArrayRange())
			m_objectList->addId(jObjectId.ToString());
	}
	m_objectList->attach("player", "inv");
	m_propertyList->attach("game", "globals");
}

void Game::quit()
{
	m_quitting = true;
}

bool Game::isQuitting()
{
	return m_quitting;
}

void Game::spawnNotification(const std::string &message, int durationMs)
{
	m_notificationManager->spawn(message, durationMs);
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

std::shared_ptr<ScriptManager> Game::getScriptManager()
{
	return m_scriptManager;
}

std::shared_ptr<SaveData> Game::getSaveData()
{
	return m_saveData;
}

std::shared_ptr<TimerManager> Game::getTimerManager()
{
	return m_timerManager;
}

std::shared_ptr<NotificationManager> Game::getNotificationManager()
{
	return m_notificationManager;
}

std::shared_ptr<TextLog> Game::getTextLog()
{
	return m_textLog;
}


GameManager::GameManager()
	: m_defaultGame(std::make_shared<Game>())
{
	setDefault();
}

GameManager &GameManager::instance()
{
	static GameManager obj;
	return obj;
}

std::shared_ptr<Game> GameManager::getActive() const
{
	return m_activeGame;
}

void GameManager::setDefault()
{
	setActive(m_defaultGame);
}

void GameManager::setActive(std::shared_ptr<Game> game)
{
	m_activeGame = game;
}


} // namespace NovelTea
