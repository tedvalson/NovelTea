#include <NovelTea/ObjectList.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/SaveData.hpp>
#include <string>

namespace NovelTea
{

ObjectList::ObjectList(std::shared_ptr<SaveData> saveData)
	: m_saveData(saveData)
{
}

bool ObjectList::addId(const std::string &objectId)
{
	return add(m_saveData->get<Object>(objectId));
}

bool ObjectList::removeId(const std::string &objectId)
{
	return remove(m_saveData->get<Object>(objectId));
}

bool ObjectList::add(std::shared_ptr<Object> object)
{
	if (!object || object->getId().empty())
		return false;
	m_objects.push_back(object);
	saveChanges();
	return true;
}

bool ObjectList::remove(std::shared_ptr<Object> object)
{
	if (!object || object->getId().empty())
		return false;
	for (int i = 0; i < m_objects.size(); ++i)
		if (object->getId() == m_objects[i]->getId())
		{
			m_objects.erase(m_objects.begin() + i);
			saveChanges();
			return true;
		}
	return false;
}

void ObjectList::clear()
{
	m_objects.clear();
	saveChanges();
}

bool ObjectList::contains(const std::shared_ptr<Object> &object)
{
	return containsId(object->getId());
}

bool ObjectList::containsId(const std::string &objectId)
{
	for (auto &object : m_objects)
		if (object->getId() == objectId)
			return true;
	return false;
}

void ObjectList::attach(const std::string &type, const std::string &id)
{
	m_attachedType = type;
	m_attachedId = id;
	auto &j = m_saveData->data()[ID::objectLocations][type][id];

	// If no object in list, load from SaveData.
	// Otherwise, save the existing ones.
	if (m_objects.empty())
	{
		for (auto &jobjectId : j.ArrayRange())
			m_objects.push_back(m_saveData->get<Object>(jobjectId.ToString()));
	}
	else
		saveChanges();
}

void ObjectList::saveChanges()
{
	if (m_attachedType.empty())
		return;

	auto jobjects = sj::Array();
	for (auto &object : m_objects)
		if (!object->getId().empty())
			jobjects.append(object->getId());

	m_saveData->data()[ID::objectLocations][m_attachedType][m_attachedId] = jobjects;
}

void ObjectList::sync()
{
	if (!m_attachedType.empty()) {
		m_objects.clear();
		attach(m_attachedType, m_attachedId);
	}
}

std::vector<std::shared_ptr<Object> > ObjectList::objects() const
{
	return m_objects;
}

} // namespace NovelTea
