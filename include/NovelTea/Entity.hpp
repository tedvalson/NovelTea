#ifndef NOVELTEA_ENTITY_HPP
#define NOVELTEA_ENTITY_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Entity : public JsonSerializable
{
public:
	virtual size_t jsonSize() const = 0;
	virtual void loadJson(const json &j) = 0;

	bool fromJson(const json &j) override;

	ADD_ACCESSOR(std::string, Id, m_id)
	ADD_ACCESSOR(std::string, ParentId, m_parentId)

protected:
	std::string m_id;
	std::string m_parentId;
};

} // namespace NovelTea

#endif // NOVELTEA_ENTITY_HPP
