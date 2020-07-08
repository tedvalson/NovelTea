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
	Verb();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	static constexpr auto id = "verb";

	std::string getActionText(std::vector<std::shared_ptr<Object>> objects, std::string blankStr = "____") const;
	std::string getActionText(std::vector<std::string> objectIds, std::string blankStr = "____") const;

	ADD_ACCESSOR(std::string, Name, m_name)
	ADD_ACCESSOR(int, ObjectCount, m_objectCount)
	ADD_ACCESSOR(std::vector<std::string>, ActionStructure, m_actionStructure)
	ADD_ACCESSOR(std::string, DefaultScriptSuccess, m_defaultScriptSuccess)
	ADD_ACCESSOR(std::string, DefaultScriptFailure, m_defaultScriptFailure)

private:
	std::string m_name;
	int m_objectCount;
	std::vector<std::string> m_actionStructure;
	std::string m_defaultScriptSuccess;
	std::string m_defaultScriptFailure;
};

} // namespace NovelTea

#endif // NOVELTEA_VERB_HPP
