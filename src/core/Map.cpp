#include <NovelTea/Map.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

Map::Map()
{
}

size_t Map::jsonSize() const
{
	return 7;
}

json Map::toJson() const
{
	auto jrooms = sj::Array();
	for (auto &room : m_rooms) {
		auto jroomIds = sj::Array();
		for (auto &id : room->roomIds)
			jroomIds.append(id);
		auto& r = room->rect;
		jrooms.append(sj::Array(room->name, r.left, r.top, r.width, r.height,
								jroomIds, room->script, static_cast<int>(room->style)));
	}
	auto jconnections = sj::Array();
	for (auto &c : m_connections) {
		jconnections.append(sj::Array(
			c->roomStart, c->roomEnd,
			c->portStart.x, c->portStart.y,
			c->portEnd.x, c->portEnd.y,
			c->script, static_cast<int>(c->style)
		));
	}

	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_defaultRoomScript,
		m_defaultPathScript,
		jrooms,
		jconnections
	);
	return j;
}

void Map::loadJson(const json &j)
{
	m_rooms.clear();
	m_connections.clear();
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_defaultRoomScript = j[3].ToString();
	m_defaultPathScript = j[4].ToString();
	// Rooms
	for (auto &jroom : j[5].ArrayRange()) {
		if (jroom.size() != 8)
			continue;
		std::vector<std::string> roomIds;
		for (auto &jroomId : jroom[5].ArrayRange())
			roomIds.push_back(jroomId.ToString());
		sf::IntRect rect(jroom[1].ToInt(), jroom[2].ToInt(), jroom[3].ToInt(), jroom[4].ToInt());
		addRoom(jroom[0].ToString(), rect, roomIds, jroom[6].ToString(), static_cast<RoomStyle>(jroom[7].ToInt()));
	}
	// Connections
	for (auto &jconn : j[6].ArrayRange()) {
		if (jconn.size() != 8)
			continue;
		addConnection(jconn[0].ToInt(), jconn[1].ToInt(),
			sf::Vector2i(jconn[2].ToInt(), jconn[3].ToInt()),
			sf::Vector2i(jconn[4].ToInt(), jconn[5].ToInt()),
			jconn[6].ToString(), static_cast<ConnectionStyle>(jconn[7].ToInt()));
	}
}

EntityType Map::entityType() const
{
	return EntityType::Map;
}

int Map::addRoom(const std::string &name, const sf::IntRect &rect, const std::vector<std::string> &roomIds, const std::string &script, RoomStyle style)
{
	auto room = new MapRoom {name, rect, roomIds, script, style};
	m_rooms.emplace_back(room);
	return m_rooms.size() - 1;
}

void Map::addConnection(int roomStart, int roomEnd, const sf::Vector2i &portStart, const sf::Vector2i &portEnd, const std::string &script, ConnectionStyle style)
{
	if (roomStart >= m_rooms.size() || roomEnd >= m_rooms.size())
		return;
	auto c = new MapConnection {roomStart, roomEnd, portStart, portEnd, script, style};
	m_connections.emplace_back(c);
}

bool Map::evalVisibility(std::shared_ptr<MapRoom> &room) const
{
	auto result = true;
	auto script = getDefaultRoomScript();
	if (!room->script.empty())
		script = room->script;
	if (!script.empty()) {
		script = "function _f(roomIds){\n"+script+"\nreturn true;}";
		try {
			result = ActiveGame->getScriptManager()->call<bool>(script, "_f", room->roomIds);
		} catch (std::exception &e) {
			std::cerr << "evalVisibility Room (" << room->name << ") " << e.what() << std::endl;
			return false;
		}
	}
	return result;
}

bool Map::evalVisibility(std::shared_ptr<MapConnection> &connection) const
{
	auto result = true;
	auto script = getDefaultPathScript();
	if (!connection->script.empty())
		script = connection->script;
	if (!script.empty()) {
		script = "function _f(startRoomIds,endRoomIds){\n"+script+"\nreturn true;}";
		try {
			auto& roomStart = m_rooms[connection->roomStart];
			auto& roomEnd = m_rooms[connection->roomEnd];
			result = ActiveGame->getScriptManager()->call<bool>(script, "_f", roomStart->roomIds, roomEnd->roomIds);
		} catch (std::exception &e) {
			std::cerr << "evalVisibility Path: " << e.what() << std::endl;
			return false;
		}
	}
	return result;
}

// Code/logic duplicated in Connection::checkDoorway()
#define BOTTOM(r) (r.top + r.height)
#define RIGHT(r) (r.left + r.width)
bool Map::checkForDoor(const MapConnection &c, sf::FloatRect &doorRect) const
{
	bool isDoor = false;
	auto& rectStart = m_rooms[c.roomStart]->rect;
	auto& rectEnd = m_rooms[c.roomEnd]->rect;
	sf::Vector2i pStart(rectStart.left + c.portStart.x, rectStart.top + c.portStart.y);
	sf::Vector2i pEnd(rectEnd.left + c.portEnd.x, rectEnd.top + c.portEnd.y);
	doorRect = sf::FloatRect(std::min(pStart.x, pEnd.x),
						 std::min(pStart.y, pEnd.y),
						 std::abs(pStart.x - pEnd.x) + 1,
						 std::abs(pStart.y - pEnd.y) + 1);
	bool bothVertical = (c.portStart.x == 0 || c.portStart.x == rectStart.width-1) &&
			(c.portEnd.x == 0 || c.portEnd.x == rectEnd.width-1);
	if (bothVertical && (RIGHT(rectStart) == rectEnd.left || rectStart.left == RIGHT(rectEnd)) &&
			(doorRect.top >= rectStart.top && doorRect.top >= rectEnd.top) &&
			(BOTTOM(doorRect) <= BOTTOM(rectStart) && BOTTOM(doorRect) <= BOTTOM(rectEnd)))
		isDoor = true;
	if (!isDoor) {
		bool bothHorizontal = (c.portStart.y == 0 || c.portStart.y == rectStart.height-1) &&
				(c.portEnd.y == 0 || c.portEnd.y == rectEnd.height-1);
		if (bothHorizontal && (rectStart.top == BOTTOM(rectEnd) || BOTTOM(rectStart) == rectEnd.top) &&
				(doorRect.left >= std::max(rectStart.left, rectEnd.left)) &&
				(RIGHT(doorRect) <= std::min(RIGHT(rectStart), RIGHT(rectEnd))))
			isDoor = true;
	}
	return isDoor;
}

} // namespace NovelTea
