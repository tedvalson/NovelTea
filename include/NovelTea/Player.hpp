#ifndef NOVELTEA_PLAYER_HPP
#define NOVELTEA_PLAYER_HPP

#include <NovelTea/Entity.hpp>
#include <NovelTea/Utils.hpp>
#include <queue>

namespace NovelTea
{

class ObjectList;
class Room;

class Player
{
public:
	static Player &instance();

	void reset();

	void setRoomId(const std::string &roomId);
	const std::string &getRoomId() const;

	void pushNextEntity(std::shared_ptr<Entity> entity);
	void pushNextEntityJson(json jentity);
	std::shared_ptr<Entity> popNextEntity();

	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(std::shared_ptr<Room>, Room, m_room)

private:
	Player();
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<Room> m_room;
	std::string m_roomId;
	std::queue<std::shared_ptr<Entity>> m_entityQueue;
};

} // namespace NovelTea

#endif // NOVELTEA_PLAYER_HPP
