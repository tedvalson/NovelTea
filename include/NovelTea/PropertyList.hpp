#ifndef NOVELTEA_PROPERTYLIST_HPP
#define NOVELTEA_PROPERTYLIST_HPP

#include <NovelTea/Utils.hpp>
#include <dukglue/dukglue.h>
#include <NovelTea/json.hpp>
#include <vector>

namespace NovelTea
{

class Object;

class PropertyList
{
public:
	PropertyList();

	DukValue get(const std::string &key, const DukValue &defaultValue) const;
	void set(const std::string &key, const DukValue &value);
	bool contains(const std::string &key) const;

	void attach(const std::string &type, const std::string &id);
	void saveChanges();

private:
	std::string m_attachedType;
	std::string m_attachedId;
	sj::JSON m_projectProperties;
	sj::JSON m_savedProperties;
};

} // namespace NovelTea

#endif // NOVELTEA_PROPERTYLIST_HPP
