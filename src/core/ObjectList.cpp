#include <NovelTea/ObjectList.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Object.hpp>

namespace NovelTea
{

ObjectList::ObjectList()
{
}

bool ObjectList::addId(const std::string &objectId)
{
	return add(Save.get<Object>(objectId));
}

bool ObjectList::removeId(const std::string &objectId)
{
	return remove(Save.get<Object>(objectId));
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

bool ObjectList::contains(const std::string &objectId)
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
	auto &j = Save.data()[ID::objectLocations][type][id];

	// If no object in list, load from SaveData.
	// Otherwise, save the existing ones.
	if (m_objects.empty())
	{
		for (auto &jobjectId : j)
			m_objects.push_back(Save.get<Object>(jobjectId));
	}
	else
		saveChanges();
}

void ObjectList::saveChanges()
{
	if (m_attachedType.empty())
		return;

	auto jobjects = json::array();
	for (auto &object : m_objects)
		if (!object->getId().empty())
			jobjects.push_back(object->getId());

	Save.data()[ID::objectLocations][m_attachedType][m_attachedId] = jobjects;
}

} // namespace NovelTea
