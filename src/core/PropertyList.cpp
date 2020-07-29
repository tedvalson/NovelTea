#include <NovelTea/PropertyList.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Object.hpp>
#include <iostream>

namespace NovelTea
{

PropertyList::PropertyList()
	: m_properties(sj::Object())
{
}

DukValue PropertyList::get(const std::string &key, const DukValue &defaultValue) const
{
	// TODO: throw error if not attached

	if (!contains(key))
	{
		auto parentId = Save.getParentId(m_attachedType, m_attachedId);
		if (parentId.empty())
			return defaultValue;

		auto parentPropertyList = std::make_shared<PropertyList>();
		parentPropertyList->attach(m_attachedType, parentId);
		return parentPropertyList->get(key, defaultValue);
	}

	auto ctx = defaultValue.context();
	auto fn = dukglue_peval<DukValue>(ctx, "_jsonGet");
	auto result = dukglue_pcall<DukValue>(ctx, fn, m_properties.dump(), key);
	return (result.type() == DukValue::UNDEFINED) ? defaultValue : result;
}

void PropertyList::set(const std::string &key, const DukValue &value)
{
	auto &j = m_properties[key];

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
	return m_properties.hasKey(key);
}

void PropertyList::attach(const std::string &type, const std::string &id)
{
	m_attachedType = type;
	m_attachedId = id;

	// If no properties in list, load from SaveData.
	// Otherwise, save the existing ones.
	if (m_properties.IsEmpty())
		m_properties = Save.data()[ID::properties][type][id];
	else
		saveChanges();
}

void PropertyList::saveChanges()
{
	if (m_attachedType.empty())
		return;

	Save.data()[ID::properties][m_attachedType][m_attachedId] = m_properties;
}

} // namespace NovelTea
