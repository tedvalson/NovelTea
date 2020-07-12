
#include <NovelTea/Player.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Room.hpp>
#include <iostream>

namespace NovelTea
{

Player::Player()
	: m_room(nullptr)
	, m_objectList(nullptr)
{
	reset();
}

Player &Player::instance()
{
	static Player player;
	return player;
}

json Player::toJson() const
{
	auto j = json::array({
//		m_name
	});
	return j;
}

bool Player::fromJson(const json &j)
{
//	m_name = j[0];
	return true;
}

void Player::reset()
{
	if (!m_room || m_room->getId().empty())
		m_room = std::make_shared<Room>();
	else
		m_room = Save.get<Room>(m_room->getId());
	m_objectList = std::make_shared<ObjectList>();
	m_objectList->attach("player", "inv");
}

void Player::setRoomId(const std::string &roomId)
{
	m_roomId = roomId;
	m_room = Save.get<Room>(roomId);
}

const std::string &Player::getRoomId() const
{
	return m_roomId;
}

} // namespace NovelTea
