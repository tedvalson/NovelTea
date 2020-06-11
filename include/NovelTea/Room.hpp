#ifndef NOVELTEA_ROOM_HPP
#define NOVELTEA_ROOM_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Room : public JsonSerializable
{
public:
	struct RoomObject {
		std::string idName;
		bool placeInRoom;
	};

	Room();
	~Room();
	json toJson() const override;
	bool fromJson(const json &j) override;

	ADD_ACCESSOR(std::string, Name, m_name)
	ADD_ACCESSOR(std::string, Description, m_description)
	ADD_ACCESSOR(std::vector<RoomObject>, Objects, m_objects)

private:
	std::string m_name;
	std::string m_description;
	std::vector<RoomObject> m_objects;
};

} // namespace NovelTea

#endif // NOVELTEA_ROOM_HPP
