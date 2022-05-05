#ifndef NOVELTEA_GAME_HPP
#define NOVELTEA_GAME_HPP

#include <NovelTea/Entity.hpp>
#include <NovelTea/Utils.hpp>
#include <queue>

#define GMan NovelTea::GameManager::instance()
#define ActiveGame GMan.getActive()
#define GSave ActiveGame->getSaveData()

namespace NovelTea
{

using MessageCallback = std::function<void(const std::vector<std::string> &messageArray, const DukValue &callback)>;
using SaveCallback = std::function<void()>;

class ObjectList;
class Map;
class Room;
class SaveData;
class ScriptManager;
class NotificationManager;
class TextLog;
class TimerManager;

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

	void setMapId(const std::string &mapId);
	const std::string getMapId() const;
	void setMap(const std::shared_ptr<Map> &map);
	const std::shared_ptr<Map> &getMap() const;

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
	bool isQuitting() const;

	void enableNavigation();
	void disableNavigation();
	bool isNavigationEnabled() const;

	void enableSave();
	void disableSave();
	bool isSaveEnabled() const;

	void spawnNotification(const std::string &message, bool addToLog = true, int durationMs = 0);

	void execMessageCallback(const std::vector<std::string> &messageArray, const DukValue &callback);
	void execMessageCallbackLog(const std::vector<std::string> &messageArray, const DukValue &callback);

	std::shared_ptr<ScriptManager> getScriptManager();
	std::shared_ptr<SaveData> getSaveData();
	std::shared_ptr<TimerManager> getTimerManager();
	std::shared_ptr<NotificationManager> getNotificationManager();
	std::shared_ptr<TextLog> getTextLog();

	ADD_ACCESSOR(bool, AutosaveEnabled, m_autosaveEnabled)
	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(MessageCallback, MessageCallback, m_messageCallback)
	ADD_ACCESSOR(SaveCallback, SaveCallback, m_saveCallback)

private:
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<PropertyList> m_propertyList;
	std::shared_ptr<Map> m_map;
	std::shared_ptr<Room> m_room;
	std::queue<std::shared_ptr<Entity>> m_entityQueue;
	bool m_autosaveEnabled;
	bool m_quitting;
	bool m_navigationEnabled;
	bool m_saveEnabled;
	bool m_initialized;

	MessageCallback m_messageCallback;
	SaveCallback m_saveCallback;

	std::shared_ptr<SaveData> m_saveData;
	std::shared_ptr<TimerManager> m_timerManager;
	std::shared_ptr<NotificationManager> m_notificationManager;
	std::shared_ptr<TextLog> m_textLog;
	std::shared_ptr<ScriptManager> m_scriptManager; // Initialize last
};


class GameManager
{
public:
	static GameManager &instance();

	std::shared_ptr<Game> getActive() const;
	std::shared_ptr<Game> getDefault() const;
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
