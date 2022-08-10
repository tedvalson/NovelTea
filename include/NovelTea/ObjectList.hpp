#ifndef NOVELTEA_OBJECTLIST_HPP
#define NOVELTEA_OBJECTLIST_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/Game.hpp>
#include <cstring>
#include <memory>
#include <vector>

namespace NovelTea
{

class Object;

struct ObjectItem {
	ObjectItem(std::shared_ptr<Object> object, int count = 1)
		:object(object), count(count)
	{}
	std::shared_ptr<Object> object;
	int count = 1;
};

class ObjectList : public ContextObject
{
public:
	ObjectList(Context* context);

	bool add(std::shared_ptr<Object> object);
	bool addCount(std::shared_ptr<Object> object, int count);
	bool addId(const std::string &objectId);
	bool addIdCount(const std::string &objectId, int count);

	bool remove(std::shared_ptr<Object> object);
	bool removeCount(std::shared_ptr<Object> object, int count);
	bool removeId(const std::string &objectId);
	bool removeIdCount(const std::string &objectId, int count);

	bool contains(const std::shared_ptr<Object> &object) const;
	bool containsCount(const std::shared_ptr<Object> &object, int count) const;
	bool containsId(const std::string &objectId) const;
	bool containsIdCount(const std::string &objectId, int count) const;

	std::vector<std::shared_ptr<Object>> getObjects() const;

	int count(const std::shared_ptr<Object> &object) const;
	int countId(const std::string &objectId) const;

	void attach(const std::string &type, const std::string &id);
	void clear();
	void saveChanges();
	void sync();

	std::vector<std::shared_ptr<ObjectItem>> items() const;

private:
	std::string m_attachedType;
	std::string m_attachedId;
	std::vector<std::shared_ptr<ObjectItem>> m_items;
};

} // namespace NovelTea

#endif // NOVELTEA_OBJECTLIST_HPP
