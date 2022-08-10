#ifndef NOVELTEA_VERB_HPP
#define NOVELTEA_VERB_HPP

#include <NovelTea/Entity.hpp>
#include <vector>

namespace NovelTea
{

class Object;

class Verb : public Entity
{
public:
	Verb(Context* context);
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Verb;
	const std::string entityId() const override {return id;}

	bool checkConditionScript(const std::string &verbId, const std::string &objectId);

	std::string getActionText(std::vector<std::shared_ptr<Object>> objects, std::string blankStr = "____") const;
	std::string getActionText(std::vector<std::string> objectIds, std::string blankStr = "____") const;

	ADD_ACCESSOR(std::string, Name, m_name)
	ADD_ACCESSOR(int, ObjectCount, m_objectCount)
	ADD_ACCESSOR(std::vector<std::string>, ActionStructure, m_actionStructure)
	ADD_ACCESSOR(std::string, ScriptDefault, m_scriptDefault)
	ADD_ACCESSOR(std::string, ScriptConditional, m_scriptConditional)

private:
	std::string m_name;
	int m_objectCount;
	std::vector<std::string> m_actionStructure;
	std::string m_scriptDefault;
	std::string m_scriptConditional;
};

} // namespace NovelTea

#endif // NOVELTEA_VERB_HPP
