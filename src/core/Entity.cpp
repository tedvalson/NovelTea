#include <NovelTea/Entity.hpp>
#include <iostream>

namespace NovelTea
{

Entity::Entity()
	: m_properties(sj::Object())
	, m_propertyList(std::make_shared<PropertyList>())
{

}

bool Entity::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != jsonSize())
		return false;

	try
	{
		loadJson(j);
		m_propertyList->attach(entityId(), m_id);
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "Entity::fromJson " << e.what() << std::endl;
		return false;
	}
}

void Entity::setId(const std::string &id)
{
	m_id = id;
	m_propertyList->attach(entityId(), id);
}

const std::string &Entity::getId() const
{
	return m_id;
}

const std::shared_ptr<PropertyList> &Entity::getPropertyList() const
{
	return m_propertyList;
}

DukValue Entity::prop(const std::string &key, const DukValue &defaultValue)
{
	return m_propertyList->get(key, defaultValue);
}

void Entity::setProp(const std::string &key, const DukValue &value)
{
	m_propertyList->set(key, value);
}

} // namespace NovelTea
