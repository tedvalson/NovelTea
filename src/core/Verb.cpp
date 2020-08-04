#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Game.hpp>

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
		m_defaultScriptSuccess,
		m_defaultScriptFailure,
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
	m_defaultScriptSuccess = j[5].ToString();
	m_defaultScriptFailure = j[6].ToString();

	m_actionStructure.clear();
	for (auto &jpart : j[7].ArrayRange())
		m_actionStructure.push_back(jpart.ToString());
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
		objects.push_back(GSave.get<Object>(objectId));
	return getActionText(objects, blankStr);
}

} // namespace NovelTea
