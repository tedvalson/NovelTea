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
		int type;
		bool visited;
	};

	struct MapConnection {
		int roomStart;
		int roomEnd;
		sf::Vector2i portStart;
		sf::Vector2i portEnd;
	};

	struct MapPath {
		sf::Vector2f start;
		sf::Vector2f end;
		int type;
	};

	struct MapDoorway {
		sf::FloatRect rect;
		int type;
	};

	Map();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Map;
	const std::string entityId() const override {return id;}

	int addRoom(const std::string &name, const sf::IntRect &rect);
	void addConnection(int roomStart, int roomEnd, const sf::Vector2i &portStart, const sf::Vector2i &portEnd);
	void refresh();

	const UniqueVector<MapRoom> &getRooms() const {return m_rooms;}
	const UniqueVector<MapConnection> &getConnections() const {return m_connections;}
	const UniqueVector<MapPath> &getPaths() const {return m_paths;}
	const UniqueVector<MapDoorway> &getDoorways() const {return m_doorways;}

private:
	UniqueVector<MapRoom> m_rooms;
	UniqueVector<MapConnection> m_connections;
	UniqueVector<MapPath> m_paths;
	UniqueVector<MapDoorway> m_doorways;
};

} // namespace NovelTea

#endif // NOVELTEA_MAP_HPP
