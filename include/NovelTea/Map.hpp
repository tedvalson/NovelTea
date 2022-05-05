#ifndef NOVELTEA_MAP_HPP
#define NOVELTEA_MAP_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class Map : public Entity
{
public:

	struct MapRoom {
		std::string name;
		sf::IntRect rect;
		std::vector<std::string> roomIds;
		std::string script;
		int type;
	};

	struct MapConnection {
		int roomStart;
		int roomEnd;
		sf::Vector2i portStart;
		sf::Vector2i portEnd;
		std::string script;
		int type;
	};


	Map();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Map;
	const std::string entityId() const override {return id;}

	int addRoom(const std::string &name, const sf::IntRect &rect, const std::vector<std::string>& roomIds, const std::string &script, int type);
	void addConnection(int roomStart, int roomEnd, const sf::Vector2i &portStart,
					   const sf::Vector2i &portEnd, const std::string &script, int type);
	void refresh();

	const UniqueVector<MapRoom> &getRooms() const {return m_rooms;}
	const UniqueVector<MapConnection> &getConnections() const {return m_connections;}
//	const UniqueVector<MapPath> &getPaths() const {return m_paths;}
//	const UniqueVector<MapDoorway> &getDoorways() const {return m_doorways;}

	ADD_ACCESSOR(std::string, DefaultRoomScript, m_defaultRoomScript)

private:
	bool m_showOnlyVisited;
	std::string m_defaultRoomScript;
	std::string m_defaultConnectionScript;
	UniqueVector<MapRoom> m_rooms;
	UniqueVector<MapConnection> m_connections;
//	UniqueVector<MapPath> m_paths;
//	UniqueVector<MapDoorway> m_doorways;
};

} // namespace NovelTea

#endif // NOVELTEA_MAP_HPP
