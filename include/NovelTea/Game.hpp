#ifndef NOVELTEA_GAME_HPP
#define NOVELTEA_GAME_HPP

#include <NovelTea/Entity.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <queue>

#define GMan NovelTea::GameManager::instance()
#define ActiveGame GMan.getActive()
#define GSave ActiveGame->getSaveData()

namespace NovelTea
{

using MessageCallback = std::function<void(const std::vector<std::string> &messageArray, const DukValue &callback)>;
using SaveCallback = std::function<void()>;

class ObjectList;
class Room;

class Game
{
public:
	Game();
	~Game();

	void initialize();
	void reset();

	void setRoomId(const std::string &roomId);
	void setRoom(const std::shared_ptr<Room> &room);
	const std::shared_ptr<Room> &getRoom() const;

	DukValue prop(const std::string &key, const DukValue &defaultValue);
	void setProp(const std::string &key, const DukValue &value);

	void pushNextEntity(std::shared_ptr<Entity> entity);
	void pushNextEntityJson(json jentity);
	std::shared_ptr<Entity> popNextEntity();

	void save(int slot);
	bool load(int slot);
	bool loadLast();
	void autosave();
	void syncToSave();

	void quit();
	bool isQuitting();

	void spawnNotification(const std::string &message, int durationMs = 0);

	void execMessageCallback(const std::vector<std::string> &messageArray, const DukValue &callback);

	ScriptManager &getScriptManager();
	SaveData &getSaveData();
	TimerManager &getTimerManager();
	NotificationManager &getNotificationManager();

	ADD_ACCESSOR(bool, AutosaveEnabled, m_autosaveEnabled)
	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(MessageCallback, MessageCallback, m_messageCallback)
	ADD_ACCESSOR(SaveCallback, SaveCallback, m_saveCallback)

private:
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<PropertyList> m_propertyList;
	std::shared_ptr<Room> m_room;
	std::string m_roomId;
	std::queue<std::shared_ptr<Entity>> m_entityQueue;
	bool m_autosaveEnabled;
	bool m_quitting;

	MessageCallback m_messageCallback;
	SaveCallback m_saveCallback;

	SaveData m_saveData;
	TimerManager m_timerManager;
	ScriptManager m_scriptManager;
	NotificationManager m_notificationManager;
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
