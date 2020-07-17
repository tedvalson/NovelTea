#ifndef NOVELTEA_ACTION_HPP
#define NOVELTEA_ACTION_HPP

#include <NovelTea/Entity.hpp>
#include <vector>

namespace NovelTea
{

class Action : public Entity
{
public:
	Action();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	void setVerbObjectCombo(const json &j);
	json getVerbObjectCombo() const;

	static std::shared_ptr<Action> find(const std::string &verbId, const std::vector<std::string> &objectIds);

	static constexpr auto id = "action";
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(std::string, VerbId, m_verbId)
	ADD_ACCESSOR(std::string, Script, m_script)
	ADD_ACCESSOR(std::vector<std::string>, ObjectIds, m_objectIds)
	ADD_ACCESSOR(bool, PositionDependent, m_positionDependent)

private:
	std::string m_verbId;
	std::string m_script;
	std::vector<std::string> m_objectIds;
	bool m_positionDependent;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTION_HPP
