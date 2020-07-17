#ifndef NOVELTEA_PROPERTYLIST_HPP
#define NOVELTEA_PROPERTYLIST_HPP

#include <NovelTea/Utils.hpp>
#include <dukglue/dukglue.h>
#include <json.hpp>
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
	nlohmann::json m_properties;
};

} // namespace NovelTea

#endif // NOVELTEA_PROPERTYLIST_HPP
