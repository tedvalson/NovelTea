#ifndef NOVELTEA_GAME_HPP
#define NOVELTEA_GAME_HPP

#include <NovelTea/Entity.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Utils.hpp>
#include <queue>

#define GMan NovelTea::GameManager::instance()
#define ActiveGame GMan.getActive()
#define GSave ActiveGame->getSaveData()

namespace NovelTea
{

class ObjectList;
class Room;

class Game
{
public:
	Game();

	void reset();

	void setRoomId(const std::string &roomId);
	const std::string &getRoomId() const;

	void pushNextEntity(std::shared_ptr<Entity> entity);
	void pushNextEntityJson(json jentity);
	std::shared_ptr<Entity> popNextEntity();

	ScriptManager &getScriptManager();
	SaveData &getSaveData();

	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(std::shared_ptr<Room>, Room, m_room)

private:
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<Room> m_room;
	std::string m_roomId;
	std::queue<std::shared_ptr<Entity>> m_entityQueue;

	SaveData m_saveData;
	ScriptManager m_scriptManager;
};


class GameManager
{
public:
	static GameManager &instance();

	std::shared_ptr<Game> getActive() const;
	void setActive(std::shared_ptr<Game> game);
	void setDefault();

protected:
	GameManager();
private:
	std::shared_ptr<Game> m_activeGame;
	std::shared_ptr<Game> m_defaultGame;
};

} // namespace NovelTea

#endif // NOVELTEA_GAME_HPP
