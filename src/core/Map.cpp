#include <NovelTea/Map.hpp>

namespace NovelTea
{

Map::Map()
{
}

size_t Map::jsonSize() const
{
	return 5;
}

json Map::toJson() const
{
	auto jrooms = sj::Array();
	for (auto &room : m_rooms) {
		auto& r = room->rect;
		jrooms.append(sj::Array(room->name, r.left, r.top, r.width, r.height));
	}
	auto jconnections = sj::Array();
	for (auto &c : m_connections) {
		jconnections.append(sj::Array(
			c->roomStart, c->roomEnd,
			c->portStart.x, c->portStart.y,
			c->portEnd.x, c->portEnd.y
		));
	}

	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
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
	// Rooms
	for (auto &jroom : j[3].ArrayRange()) {
		sf::IntRect rect(jroom[1].ToInt(), jroom[2].ToInt(), jroom[3].ToInt(), jroom[4].ToInt());
		addRoom(jroom[0].ToString(), rect);
	}
	// Connections
	for (auto &jconn : j[4].ArrayRange()) {
		addConnection(jconn[0].ToInt(), jconn[1].ToInt(),
			{jconn[2].ToInt(), jconn[3].ToInt()},
			{jconn[4].ToInt(), jconn[5].ToInt()});
	}
	refresh();
}

EntityType Map::entityType() const
{
	return EntityType::Map;
}

int Map::addRoom(const std::string &name, const sf::IntRect &rect)
{
	auto room = new MapRoom {name, rect, 0, false};
	m_rooms.emplace_back(room);
	return m_rooms.size() - 1;
}

void Map::addConnection(int roomStart, int roomEnd, const sf::Vector2i &portStart, const sf::Vector2i &portEnd)
{
	if (roomStart >= m_rooms.size() || roomEnd >= m_rooms.size())
		return;
	auto c = new MapConnection {roomStart, roomEnd, portStart, portEnd};
	m_connections.emplace_back(c);
}

namespace {
float convertVal(float val, float maxVal)
{
	if (val > 0) {
		if (val < maxVal - 1.f)
			val += 0.5f;
		else
			val = maxVal;
	}
	return val;
}
}

void Map::refresh()
{
	m_paths.clear();
	m_doorways.clear();
	for (auto &c : m_connections)
	{
		auto& rectStart = m_rooms[c->roomStart]->rect;
		auto& rectEnd = m_rooms[c->roomEnd]->rect;
		sf::Vector2f startPos(convertVal(c->portStart.x, rectStart.width) + rectStart.left,
							  convertVal(c->portStart.y, rectStart.height) + rectStart.top);
		sf::Vector2f endPos(convertVal(c->portEnd.x, rectEnd.width) + rectEnd.left,
							  convertVal(c->portEnd.y, rectEnd.height) + rectEnd.top);
		m_paths.emplace_back(new MapPath {startPos, endPos, 0});
	}
}

} // namespace NovelTea
