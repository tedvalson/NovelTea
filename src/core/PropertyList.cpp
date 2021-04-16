#include <NovelTea/PropertyList.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

namespace NovelTea
{

PropertyList::PropertyList()
	: m_projectProperties(sj::Object())
	, m_savedProperties(sj::Object())
{
}

DukValue PropertyList::get(const std::string &key, const DukValue &defaultValue) const
{
	// TODO: throw error if not attached

	if (!contains(key))
	{
		auto parentId = GSave->getParentId(m_attachedType, m_attachedId);
		if (parentId.empty())
			return defaultValue;

		auto parentPropertyList = std::make_shared<PropertyList>();
		parentPropertyList->attach(m_attachedType, parentId);
		return parentPropertyList->get(key, defaultValue);
	}

	auto props = m_savedProperties.hasKey(key) ? &m_savedProperties : &m_projectProperties;
	auto ctx = defaultValue.context();
	auto fn = dukglue_peval<DukValue>(ctx, "_jsonGet");
	auto result = dukglue_pcall<DukValue>(ctx, fn, props->dump(), key);
	return (result.type() == DukValue::UNDEFINED) ? defaultValue : result;
}

void PropertyList::set(const std::string &key, const DukValue &value)
{
	auto &j = m_savedProperties[key];

	if (value.type() == DukValue::NUMBER)
		j = value.as_double();
	else if (value.type() == DukValue::BOOLEAN)
		j = value.as_bool();
	else if (value.type() == DukValue::STRING)
		j = value.as_string();

	saveChanges();
}

bool PropertyList::contains(const std::string &key) const
{
	return m_savedProperties.hasKey(key) || m_projectProperties.hasKey(key);
}

void PropertyList::setValue(const std::string &key, const sj::JSON &value)
{
	m_savedProperties[key] = value;
	saveChanges();
}

const sj::JSON &PropertyList::getValue(const std::string &key) const
{
	if (m_savedProperties.hasKey(key))
		return m_savedProperties[key];
	else
		return m_projectProperties[key];
}

void PropertyList::attach(const std::string &type, const std::string &id)
{
	m_attachedType = type;
	m_attachedId = id;

	auto &jEntities = GSave->data()[ID::properties][type];
	m_projectProperties = ProjData[type][id][ID::entityProperties];

	// If no properties in list, load from SaveData.
	// Otherwise, save the existing ones.
	if (m_savedProperties.IsEmpty() && jEntities.hasKey(id))
		m_savedProperties = jEntities[id];
	else
		saveChanges();
}

void PropertyList::saveChanges()
{
	if (m_attachedType.empty())
		return;

	if (!m_savedProperties.IsEmpty())
		GSave->data()[ID::properties][m_attachedType][m_attachedId] = m_savedProperties;
}

void PropertyList::sync()
{
	if (!m_attachedType.empty()) {
		m_savedProperties = sj::Object();
		attach(m_attachedType, m_attachedId);
	}
}

} // namespace NovelTea
