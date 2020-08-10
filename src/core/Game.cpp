#include <NovelTea/Game.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Cutscene.hpp>
#include <iostream>

namespace NovelTea
{

Game::Game()
	: m_room(nullptr)
	, m_objectList(nullptr)
	, m_scriptManager(this, &m_saveData)
{
	reset();
}

Game::~Game()
{
	// TODO: Need to make sure active timers are destroyed first.
	//       shared_ptrs here may not clean up when expected?
	m_timerManager.reset();
}

void Game::reset()
{
	if (!Proj.isLoaded())
		return;

	m_saveData.reset();
	m_timerManager.reset();
	m_scriptManager.reset();

	if (!m_room || m_room->getId().empty())
		m_room = std::make_shared<Room>();
	else
		m_room = m_saveData.get<Room>(m_room->getId());
	m_objectList = std::make_shared<ObjectList>(m_saveData);
	m_objectList->attach("player", "inv");
}

void Game::setRoomId(const std::string &roomId)
{
	m_roomId = roomId;
	m_room = m_saveData.get<Room>(roomId);
}

const std::string &Game::getRoomId() const
{
	return m_roomId;
}

void Game::pushNextEntity(std::shared_ptr<Entity> entity)
{
	m_entityQueue.push(entity);
}

void Game::pushNextEntityJson(json jentity)
{
	auto type = static_cast<EntityType>(jentity[ID::selectEntityType].ToInt());
	auto idName = jentity[ID::selectEntityId].ToString();
	if (type == EntityType::Cutscene)
		pushNextEntity(m_saveData.get<Cutscene>(idName));
	else if (type == EntityType::Room)
		pushNextEntity(m_saveData.get<Room>(idName));
}

std::shared_ptr<Entity> Game::popNextEntity()
{
	if (m_entityQueue.empty())
		return nullptr;
	auto nextEntity = m_entityQueue.front();
	m_entityQueue.pop();
	return nextEntity;
}

ScriptManager &Game::getScriptManager()
{
	return m_scriptManager;
}

SaveData &Game::getSaveData()
{
	return m_saveData;
}

TimerManager &Game::getTimerManager()
{
	return m_timerManager;
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
