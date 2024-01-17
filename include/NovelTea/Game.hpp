#ifndef NOVELTEA_GAME_HPP
#define NOVELTEA_GAME_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Entity.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/Err.hpp>
#include <functional>
#include <queue>

namespace NovelTea
{

using MessageCallback = std::function<void(const std::vector<std::string> &messageArray, const DukValue &callback)>;
using SaveCallback = std::function<void()>;

class ObjectList;
class Map;
class Room;
class SaveData;
class Settings;

class Game : public ContextObject
{
public:
	Game(Context* context);
	virtual ~Game();

	static std::string SubsystemName;

	virtual bool initialize();
	virtual void reset();

	void setRoomId(const std::string &roomId);
	void setRoom(const std::shared_ptr<Room> &room);
	const std::shared_ptr<Room> &getRoom() const;

	void setMapId(const std::string &mapId);
	const std::string getMapId() const;
	void setMap(const std::shared_ptr<Map> &map);
	const std::shared_ptr<Map> &getMap() const;

	DukValue prop(const std::string &key, const DukValue &defaultValue);
	void setProp(const std::string &key, const DukValue &value);

	void pushNextEntity(std::shared_ptr<Entity> entity, const DukValue &value = DukValue());
	void pushNextEntityJson(json jentity);
	std::shared_ptr<Entity> popNextEntity();

	virtual void save(int slot);
	virtual bool load(int slot);
	virtual bool loadLast();
	virtual void autosave();
	virtual void syncToSave();

	void quit();
	bool isQuitting() const;

	virtual void spawnNotification(const std::string &message, bool addToLog = true, int durationMs = 0);

	virtual void execMessageCallback(const std::vector<std::string> &messageArray, const DukValue &callback);
	virtual void execMessageCallbackLog(const std::vector<std::string> &messageArray, const DukValue &callback);

	std::shared_ptr<Settings> getSettings() const { return m_settings; }
	std::shared_ptr<SaveData> getSaveData() const { return m_saveData; }
	std::shared_ptr<ProjectData> getProjectData() const { return m_projectData; }

	std::string getParentId(const std::string &entityType, const std::string &entityId);
	void set(std::shared_ptr<Entity> obj, const std::string &idName = std::string());

	ADD_ACCESSOR(bool, AutosaveEnabled, m_autosaveEnabled)
	ADD_ACCESSOR(bool, MinimapEnabled, m_minimapEnabled)
	ADD_ACCESSOR(bool, NavigationEnabled, m_navigationEnabled)
	ADD_ACCESSOR(bool, SaveEnabled, m_saveEnabled)
	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(MessageCallback, MessageCallback, m_messageCallback)
	ADD_ACCESSOR(SaveCallback, SaveCallback, m_saveCallback)

	template <typename T>
	std::shared_ptr<T> makeContextObject()
	{
		return std::make_shared<T>(getContext());
	}

	template <typename T>
	bool exists(const std::string &idName)
	{
		if (idName.empty())
			return false;
		return (m_projectData->data()[T::id].hasKey(idName) || m_saveData->data()[T::id].hasKey(idName));
	}

	template <typename T>
	std::shared_ptr<T> get(const std::string &idName)
	{
		auto result = std::make_shared<T>(getContext());
		if (exists<T>(idName))
		{
			if (m_saveData->data()[T::id].hasKey(idName))
				result->fromJson(m_saveData->data()[T::id][idName]);
			else
				result->fromJson(m_projectData->data()[T::id][idName]);
		} else {
			if (!idName.empty())
				warn() << "Entity doesn't exist - " << T::id << " '" << idName << "'" << std::endl;
		}
		return result;
	}

protected:
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<PropertyList> m_propertyList;
	std::shared_ptr<Map> m_map;
	std::shared_ptr<Room> m_room;
	std::queue<std::pair<std::shared_ptr<Entity>, DukValue>> m_entityQueue;
	bool m_autosaveEnabled;
	bool m_quitting;
	bool m_minimapEnabled;
	bool m_navigationEnabled;
	bool m_saveEnabled;
	bool m_initialized;

	MessageCallback m_messageCallback;
	SaveCallback m_saveCallback;

	std::shared_ptr<Settings> m_settings;
	std::shared_ptr<ProjectData> m_projectData;
	std::shared_ptr<SaveData> m_saveData;
};

} // namespace NovelTea

#endif // NOVELTEA_GAME_HPP
