#ifndef NOVELTEA_ACTION_HPP
#define NOVELTEA_ACTION_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <vector>

namespace NovelTea
{

class Action : public JsonSerializable
{
public:
	Action();
	json toJson() const override;
	bool fromJson(const json &j) override;

	void setVerbObjectCombo(const json &j);
	json getVerbObjectCombo() const;

	static constexpr auto id = "action";

	ADD_ACCESSOR(std::string, VerbId, m_verbId)
	ADD_ACCESSOR(std::string, Script, m_script)
	ADD_ACCESSOR(std::vector<std::string>, ObjectIds, m_objectIds)
	ADD_ACCESSOR(bool, PositionDependent, m_positionDependent)
	ADD_ACCESSOR(json, Properties, m_properties)

private:
	std::string m_verbId;
	std::string m_script;
	std::vector<std::string> m_objectIds;
	bool m_positionDependent;
	json m_properties;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTION_HPP
