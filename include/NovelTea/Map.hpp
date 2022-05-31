#ifndef NOVELTEA_MAP_HPP
#define NOVELTEA_MAP_HPP

#include <SFML/Graphics/Rect.hpp>
#include <NovelTea/Entity.hpp>

namespace NovelTea
{

enum class RoomStyle : int {
	Borderless  = 0,
	SolidBorder = 1,
	DashedBorder,
};

enum class ConnectionStyle : int {
	SolidLine,
};

struct MapRoom {
	std::string name;
	sf::IntRect rect;
	std::vector<std::string> roomIds;
	std::string script;
	RoomStyle style;
};

struct MapConnection {
	int roomStart;
	int roomEnd;
	sf::Vector2i portStart;
	sf::Vector2i portEnd;
	std::string script;
	ConnectionStyle style;
};

class Map : public Entity
{
public:
	Map();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Map;
	const std::string entityId() const override {return id;}

	int addRoom(const std::string &name, const sf::IntRect &rect, const std::vector<std::string>& roomIds, const std::string &script, RoomStyle style);
	void addConnection(int roomStart, int roomEnd, const sf::Vector2i &portStart,
					   const sf::Vector2i &portEnd, const std::string &script, ConnectionStyle style);

	bool evalVisibility(std::shared_ptr<MapRoom> &room) const;
	bool evalVisibility(std::shared_ptr<MapConnection> &connection) const;

	const SharedVector<MapRoom> &getRooms() const {return m_rooms;}
	const SharedVector<MapConnection> &getConnections() const {return m_connections;}

	bool checkForDoor(const MapConnection &connection, sf::FloatRect &doorRect) const;

	ADD_ACCESSOR(std::string, DefaultRoomScript, m_defaultRoomScript)
	ADD_ACCESSOR(std::string, DefaultPathScript, m_defaultPathScript)

private:
	std::string m_defaultRoomScript;
	std::string m_defaultPathScript;
	SharedVector<MapRoom> m_rooms;
	SharedVector<MapConnection> m_connections;
};

} // namespace NovelTea

#endif // NOVELTEA_MAP_HPP
