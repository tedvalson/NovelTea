#ifndef NOVELTEA_ROOM_HPP
#define NOVELTEA_ROOM_HPP

#include <NovelTea/JsonSerializable.hpp>

namespace NovelTea
{

class Room : public JsonSerializable
{
public:
	Room();
	~Room();
	json toJson() const override;
	bool fromJson(const json &j) override;

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_ROOM_HPP
