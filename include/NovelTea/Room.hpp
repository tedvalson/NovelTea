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
	~Room();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	bool contains(const std::string &objectId);

	static json getProjectRoomObjects();

	static constexpr auto id = "room";

	void setId(const std::string &idName) override;
	const std::shared_ptr<ObjectList> &getObjectList() const;

	ADD_ACCESSOR(std::string, Name, m_name)
	ADD_ACCESSOR(std::string, Description, m_description)
	ADD_ACCESSOR(std::vector<RoomObject>, Objects, m_objects)

private:
	std::string m_name;
	std::string m_description;
	std::vector<RoomObject> m_objects;

	std::shared_ptr<ObjectList> m_objectList;
};

} // namespace NovelTea

#endif // NOVELTEA_ROOM_HPP
