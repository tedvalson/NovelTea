#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

namespace NovelTea
{

Verb::Verb()
: m_name("New Verb")
, m_objectCount(1)
, m_properties(json::object())
{
	
}

json Verb::toJson() const
{
	auto j = json::array({
		m_id,
		m_name,
		m_objectCount,
		m_defaultScriptSuccess,
		m_defaultScriptFailure,
		m_actionStructure,
		m_properties,
	});
	return j;
}

bool Verb::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 7)
		return false;

	try
	{
		m_id = j[0];
		m_name = j[1];
		m_objectCount = j[2];
		m_defaultScriptSuccess = j[3];
		m_defaultScriptFailure = j[4];
		m_properties = j[6];

		m_actionStructure.clear();
		for (auto &jpart : j[5])
			m_actionStructure.push_back(jpart);
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
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
		objects.push_back(Save.get<Object>(objectId));
	return getActionText(objects, blankStr);
}

} // namespace NovelTea
