
#include <NovelTea/Player.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Cutscene.hpp>
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

void Player::pushNextEntity(std::shared_ptr<Entity> entity)
{
	m_entityQueue.push(entity);
}

void Player::pushNextEntityJson(json jentity)
{
	auto type = jentity[ID::selectEntityType];
	auto idName = jentity[ID::selectEntityId];
	if (type == EntityType::Cutscene)
		pushNextEntity(Save.get<Cutscene>(idName));
	else if (type == EntityType::Room)
		pushNextEntity(Save.get<Room>(idName));
}

std::shared_ptr<Entity> Player::popNextEntity()
{
	if (m_entityQueue.empty())
		return nullptr;
	auto nextEntity = m_entityQueue.front();
	m_entityQueue.pop();
	return nextEntity;
}

} // namespace NovelTea
