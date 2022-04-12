#ifndef NOVELTEA_MAPROOM_HPP
#define NOVELTEA_MAPROOM_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class MapRoom : public JsonSerializable
{
public:
	MapRoom();

	json toJson() const override;
	bool fromJson(const json &j) override;

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
	int m_posX;
	int m_posY;
	std::vector<int> m_connections;
};

} // namespace NovelTea

#endif // NOVELTEA_MAPROOM_HPP
