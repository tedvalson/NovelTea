#ifndef NOVELTEA_OBJECTLIST_HPP
#define NOVELTEA_OBJECTLIST_HPP

#include <NovelTea/Utils.hpp>
#include <cstring>
#include <memory>
#include <vector>

namespace NovelTea
{

class Object;

class ObjectList
{
public:
	ObjectList();

	bool addId(const std::string &objectId);
	bool removeId(const std::string &objectId);
	bool add(std::shared_ptr<Object> object);
	bool remove(std::shared_ptr<Object> object);

	void clear();
	bool contains(const std::string &objectId);

	void attach(const std::string &type, const std::string &id);
	void saveChanges();

private:
	std::string m_attachedType;
	std::string m_attachedId;
	std::vector<std::shared_ptr<Object>> m_objects;
};

} // namespace NovelTea

#endif // NOVELTEA_OBJECTLIST_HPP