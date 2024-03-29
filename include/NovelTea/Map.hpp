#ifndef NOVELTEA_MAP_HPP
#define NOVELTEA_MAP_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

struct IntRect {
	IntRect(int left, int top, int width, int height)
		: left(left), top(top), width(width), height(height)
		{}
	int left, top, width, height;
};

struct FloatRect {
	FloatRect(){}
	FloatRect(float left, float top, float width, float height)
		: left(left), top(top), width(width), height(height)
		{}
	float left, top, width, height;
};

struct Vector2i {
	Vector2i(int x, int y) : x(x), y(y) {}
	int x, y;
};

enum class RoomStyle : int {
	Borderless = 0,
	SolidBorder = 1,
	DashedBorder,
};

enum class ConnectionStyle : int {
	None = 0,
};

struct MapRoom {
	std::string name;
	IntRect rect;
	std::vector<std::string> roomIds;
	std::string script;
	RoomStyle style;
};

struct MapConnection {
	int roomStart;
	int roomEnd;
	Vector2i portStart;
	Vector2i portEnd;
	std::string script;
	ConnectionStyle style;
};

class Map : public Entity
{
public:
	Map(Context *context);
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Map;
	const std::string entityId() const override { return id; }

	int addRoom(const std::string &name, const IntRect &rect,
				const std::vector<std::string> &roomIds,
				const std::string &script, RoomStyle style);
	void addConnection(int roomStart, int roomEnd,
					   const Vector2i &portStart,
					   const Vector2i &portEnd, const std::string &script,
					   ConnectionStyle style);

	/// Binding for ScriptManager
	int scriptAddRoom(const std::string &name, int x, int y, int w, int h,
					  const std::vector<std::string> &roomIds,
					  const std::string &script, int style);
	void scriptAddConnection(int roomStart, int roomEnd, int startX,
							 int startY, int endX, int endY,
							 const std::string &script, int style);

	bool evalVisibility(std::shared_ptr<MapRoom> &room) const;
	bool evalVisibility(std::shared_ptr<MapConnection> &connection) const;

	const SharedVector<MapRoom> &getRooms() const { return m_rooms; }
	const SharedVector<MapConnection> &getConnections() const { return m_connections; }

	bool checkForDoor(const MapConnection &connection,
					  FloatRect &doorRect) const;

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
