#ifndef NOVELTEA_MAP_HPP
#define NOVELTEA_MAP_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

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

	ADD_ACCESSOR(std::string, Content, m_content)

private:
	std::string m_content;
};

} // namespace NovelTea

#endif // NOVELTEA_MAP_HPP
