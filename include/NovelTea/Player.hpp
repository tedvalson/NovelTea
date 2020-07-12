#ifndef NOVELTEA_PLAYER_HPP
#define NOVELTEA_PLAYER_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class ObjectList;
class Room;

class Player : public JsonSerializable
{
public:
	static Player &instance();

	json toJson() const override;
	bool fromJson(const json &j) override;

	void reset();

	void setRoomId(const std::string &roomId);
	const std::string &getRoomId() const;

	ADD_ACCESSOR(std::shared_ptr<ObjectList>, ObjectList, m_objectList)
	ADD_ACCESSOR(std::shared_ptr<Room>, Room, m_room)

private:
	Player();
	std::shared_ptr<ObjectList> m_objectList;
	std::shared_ptr<Room> m_room;
	std::string m_roomId;
};

} // namespace NovelTea

#endif // NOVELTEA_PLAYER_HPP
