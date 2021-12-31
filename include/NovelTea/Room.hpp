#ifndef NOVELTEA_ROOM_HPP
#define NOVELTEA_ROOM_HPP

#include <NovelTea/Entity.hpp>
#include <NovelTea/ObjectList.hpp>

namespace NovelTea
{

class Room : public Entity
{
public:
	struct RoomObject {
		std::string idName;
		bool placeInRoom;
	};

	Room();

	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	bool contains(const std::shared_ptr<Object> &object);
	bool containsId(const std::string &objectId);

	static json getProjectRoomObjects();

	static constexpr auto id = "room";
	const std::string entityId() const override {return id;}

	void setId(const std::string &idName) override;
	const std::shared_ptr<ObjectList> &getObjectList() const;

	std::string getDescription() const;
	void sync();

	bool runScript(const std::string &script) const;
	bool runScriptBeforeEnter() const;
	void runScriptAfterEnter() const;
	bool runScriptBeforeLeave() const;
	void runScriptAfterLeave() const;

	ADD_ACCESSOR(std::string, Name, m_name)
	ADD_ACCESSOR(std::string, DescriptionRaw, m_descriptionRaw)
	ADD_ACCESSOR(std::string, ScriptBeforeEnter, m_scriptBeforeEnter)
	ADD_ACCESSOR(std::string, ScriptAfterEnter, m_scriptAfterEnter)
	ADD_ACCESSOR(std::string, ScriptBeforeLeave, m_scriptBeforeLeave)
	ADD_ACCESSOR(std::string, ScriptAfterLeave, m_scriptAfterLeave)
	ADD_ACCESSOR(std::vector<RoomObject>, Objects, m_objects)
	ADD_ACCESSOR(json, Paths, m_paths)

private:
	std::string m_name;
	std::string m_descriptionRaw;
	std::string m_scriptBeforeEnter;
	std::string m_scriptAfterEnter;
	std::string m_scriptBeforeLeave;
	std::string m_scriptAfterLeave;
	std::vector<RoomObject> m_objects;
	json m_paths;

	std::shared_ptr<ObjectList> m_objectList;
};

} // namespace NovelTea

#endif // NOVELTEA_ROOM_HPP
