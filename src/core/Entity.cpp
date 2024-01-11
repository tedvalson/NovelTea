#include <NovelTea/Entity.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/PropertyList.hpp>
#include <iostream>

namespace NovelTea
{

Entity::Entity(Context* context)
	: ContextObject(context)
	, m_properties(sj::Object())
	, m_propertyList(std::make_shared<PropertyList>(context))
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

bool Entity::hasProp(const std::string &key)
{
	return m_propertyList->contains(key);
}

void Entity::setProp(const std::string &key, const DukValue &value)
{
	m_propertyList->set(key, value);
}

void Entity::unsetProp(const std::string &key)
{
	m_propertyList->unset(key);
}

std::shared_ptr<Entity> Entity::fromEntityJson(Context* context, const json &j)
{
	auto game = context->getGame();
	auto type = static_cast<EntityType>(j[ID::selectEntityType].ToInt());
	auto idName = j[ID::selectEntityId].ToString();
	if (type == EntityType::Action)
		return game->get<Action>(idName);
	else if (type == EntityType::Cutscene)
		return game->get<Cutscene>(idName);
	else if (type == EntityType::Dialogue)
		return game->get<Dialogue>(idName);
	else if (type == EntityType::Object)
		return game->get<Object>(idName);
	else if (type == EntityType::Room)
		return game->get<Room>(idName);
	else if (type == EntityType::Script)
		return game->get<Script>(idName);
	else if (type == EntityType::Verb)
		return game->get<Verb>(idName);
	else if (type == EntityType::CustomScript) {
		auto script = std::make_shared<Script>(context);
		script->setContent(idName);
		return script;
	}
	return nullptr;
}

} // namespace NovelTea
