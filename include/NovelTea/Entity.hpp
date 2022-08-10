#ifndef NOVELTEA_ENTITY_HPP
#define NOVELTEA_ENTITY_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <dukglue/dukvalue.h>

namespace NovelTea
{

class PropertyList;
class SaveData;

class Entity : public ContextObject, public JsonSerializable
{
public:
	Entity(Context* context);
	virtual size_t jsonSize() const = 0;
	virtual void loadJson(const json &j) = 0;
	virtual const std::string entityId() const = 0;
	virtual EntityType entityType() const = 0;

	bool fromJson(const json &j) override;

	virtual void setId(const std::string &id);
	const std::string &getId() const;

	const std::shared_ptr<PropertyList> &getPropertyList() const;
	DukValue prop(const std::string &key, const DukValue &defaultValue);
	void setProp(const std::string &key, const DukValue &value);
	void unsetProp(const std::string &key);

	ADD_ACCESSOR(std::string, ParentId, m_parentId)
	ADD_ACCESSOR(json, Properties, m_properties)

	static std::shared_ptr<Entity> fromEntityJson(Context* context, const json &j);

protected:
	std::string m_id;
	std::string m_parentId;
	json m_properties;
private:
	std::shared_ptr<PropertyList> m_propertyList;
};

} // namespace NovelTea

#endif // NOVELTEA_ENTITY_HPP
