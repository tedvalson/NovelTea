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

bool ObjectList::add(std::shared_ptr<Object> object)
{
	return addCount(object, 1);
}

// Inserts new items based on object's name (making it sorted)
bool ObjectList::addCount(std::shared_ptr<Object> object, int count)
{
	if (!object || object->getId().empty())
		return false;
	bool exists = false;
	for (auto& item : m_items) {
		if (item->object->getId() == object->getId()) {
			item->count += count;
			exists = true;
			break;
		}
	}
	if (!exists) {
		int i = 0;
		for (; i < m_items.size(); ++i) {
			if (m_items[i]->object->getName() >= object->getName())
				break;
		}
		m_items.insert(m_items.begin() + i, std::make_shared<ObjectItem>(object, count));
	}

	saveChanges();
	return true;
}

bool ObjectList::addId(const std::string &objectId)
{
	return addIdCount(objectId, 1);
}

bool ObjectList::addIdCount(const std::string &objectId, int count)
{
	return addCount(m_saveData->get<Object>(objectId), count);
}

bool ObjectList::remove(std::shared_ptr<Object> object)
{
	return removeCount(object, 1);
}

bool ObjectList::removeCount(std::shared_ptr<Object> object, int count)
{
	if (!object || object->getId().empty())
		return false;
	for (int i = 0; i < m_items.size(); ++i)
	{
		auto &item = m_items[i];
		if (object->getId() == item->object->getId())
		{
			if (item->count < count)
				return false;
			else if (item->count == count)
				m_items.erase(m_items.begin() + i);
			else
				item->count -= count;

			saveChanges();
			return true;
		}
	}
	return false;
}

bool ObjectList::removeId(const std::string &objectId)
{
	return removeIdCount(objectId, 1);
}

bool ObjectList::removeIdCount(const std::string &objectId, int count)
{
	return removeCount(m_saveData->get<Object>(objectId), count);
}

bool ObjectList::contains(const std::shared_ptr<Object> &object) const
{
	return containsCount(object, 1);
}

bool ObjectList::containsCount(const std::shared_ptr<Object> &object, int count) const
{
	return containsIdCount(object->getId(), count);
}

bool ObjectList::containsId(const std::string &objectId) const
{
	return containsIdCount(objectId, 1);
}

bool ObjectList::containsIdCount(const std::string &objectId, int count) const
{
	return countId(objectId) >= count;
}

std::vector<std::shared_ptr<Object>> ObjectList::getObjects() const
{
	std::vector<std::shared_ptr<Object>> result;
	for (auto& item : m_items)
		result.push_back(item->object);
	return result;
}

int ObjectList::count(const std::shared_ptr<Object> &object) const
{
	return countId(object->getId());
}

int ObjectList::countId(const std::string &objectId) const
{
	for (auto &item : m_items)
		if (item->object->getId() == objectId)
			return item->count;
	return 0;
}

void ObjectList::attach(const std::string &type, const std::string &id)
{
	m_attachedType = type;
	m_attachedId = id;
	auto &j = m_saveData->data()[ID::objectLocations][type][id];

	// If no object in list, load from SaveData.
	// Otherwise, save the existing ones.
	if (m_items.empty())
	{
		for (auto &jitem : j.ArrayRange()) {
			auto item = new ObjectItem(m_saveData->get<Object>(jitem[0].ToString()), jitem[1].ToInt());
			m_items.emplace_back(item);
		}
	}
	else
		saveChanges();
}

void ObjectList::clear()
{
	m_items.clear();
	saveChanges();
}

void ObjectList::saveChanges()
{
	if (m_attachedType.empty())
		return;

	auto jobjects = sj::Array();
	for (auto &item : m_items)
		if (!item->object->getId().empty())
			jobjects.append(sj::Array(item->object->getId(), item->count));

	m_saveData->data()[ID::objectLocations][m_attachedType][m_attachedId] = jobjects;
}

void ObjectList::sync()
{
	if (!m_attachedType.empty()) {
		m_items.clear();
		attach(m_attachedType, m_attachedId);
	}
}

std::vector<std::shared_ptr<ObjectItem>> ObjectList::items() const
{
	return m_items;
}

} // namespace NovelTea
