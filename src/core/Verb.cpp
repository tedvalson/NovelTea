#include <NovelTea/Verb.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

Verb::Verb()
: m_name("New Verb")
, m_objectCount(1)
{
	
}

size_t Verb::jsonSize() const
{
	return 8;
}

json Verb::toJson() const
{
	auto jactionStruct = sj::Array();
	for (auto &s : m_actionStructure)
		jactionStruct.append(s);
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_name,
		m_objectCount,
		m_scriptDefault,
		m_scriptConditional,
		jactionStruct
	);
	return j;
}

void Verb::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_name = j[3].ToString();
	m_objectCount = j[4].ToInt();
	m_scriptDefault = j[5].ToString();
	m_scriptConditional = j[6].ToString();

	m_actionStructure.clear();
	for (auto &jpart : j[7].ArrayRange())
		m_actionStructure.push_back(jpart.ToString());
}

bool Verb::checkConditionScript(const std::string &verbId, const std::string &objectId)
{
	if (m_scriptConditional.empty()) {
		if (m_parentId.empty())
			return true;
		auto parentVerb = GSave->get<Verb>(m_parentId);
		return parentVerb->checkConditionScript(verbId, objectId);
	}

	try {
		auto object = GSave->get<Object>(objectId);
		auto verb = GSave->get<Verb>(verbId);
		auto script = "function _f(verb,object){\n" + m_scriptConditional + "\nreturn true;}";
		return ActiveGame->getScriptManager()->call<bool>(script, "_f", verb, object);
	} catch (std::exception &e) {
		std::cerr << "Verb::checkConditionScript " << e.what() << std::endl;
		return false;
	}
}

std::string Verb::getActionText(std::vector<std::shared_ptr<Object>> objects, std::string blankStr) const
{
	auto &actionStructure = getActionStructure();
	std::string result;
	if (!actionStructure.empty())
		result = actionStructure[0];

	for (int i = 1; i < actionStructure.size(); ++i)
	{
		auto objectStr = blankStr;
		if (i-1 < objects.size())
		{
			auto object = objects[i-1];
			if (!object->getId().empty())
			{
				objectStr = object->getName();
				if (!object->getCaseSensitive())
					std::transform(objectStr.begin(), objectStr.end(), objectStr.begin(), ::tolower);
			}
		}

		result += " " + objectStr;
		if (!actionStructure[i].empty())
			result += " " + actionStructure[i];
	}
	return result;
}

std::string Verb::getActionText(std::vector<std::string> objectIds, std::string blankStr) const
{
	std::vector<std::shared_ptr<Object>> objects;
	for (auto &objectId : objectIds)
		objects.push_back(GSave->get<Object>(objectId));
	return getActionText(objects, blankStr);
}

} // namespace NovelTea
