#ifndef NOVELTEA_PROPERTYLIST_HPP
#define NOVELTEA_PROPERTYLIST_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/Utils.hpp>
#include <dukglue/dukvalue.h>
#include <NovelTea/json.hpp>
#include <vector>

namespace NovelTea
{

class Object;

class PropertyList : public ContextObject
{
public:
	PropertyList(Context* context);

	DukValue get(const std::string &key, const DukValue &defaultValue) const;
	void set(const std::string &key, const DukValue &value);
	void unset(const std::string &key);
	bool contains(const std::string &key) const;

	void setValue(const std::string &key, const sj::JSON &value);
	const sj::JSON &getValue(const std::string &key) const;

	void attach(const std::string &type, const std::string &id);
	void saveChanges();
	void sync();

private:
	std::string m_attachedType;
	std::string m_attachedId;
	sj::JSON m_projectProperties;
	sj::JSON m_savedProperties;
};

} // namespace NovelTea

#endif // NOVELTEA_PROPERTYLIST_HPP
