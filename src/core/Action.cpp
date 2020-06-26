#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/ProjectData.hpp>
#include <iostream>

namespace NovelTea
{

Action::Action()
: m_properties(json::object())
{
	
}

json Action::toJson() const
{
	auto j = json::array({
		m_verbId,
		m_script,
		m_objectIds,
		m_properties,
	});
	return j;
}

bool Action::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 4)
		return false;

	try
	{
		m_verbId = j[0];
		m_script = j[1];
		m_properties = j[3];

		m_objectIds.clear();
		for (auto &jpart : j[2])
			m_objectIds.push_back(jpart);

		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

void Action::setVerbObjectCombo(const json &j)
{
	if (!j.is_array() || j.size() != 2 ||
			!j[1].is_array() || j[1].empty())
		return;

	auto verb = Proj.verb(j[0]);
	if (!verb)
		return;

	auto objectCount = verb->getObjectCount();
	if (j[1].size() != objectCount)
		return;

	m_verbId = j[0];
	m_objectIds.clear();
	for (auto &jobjectId : j[1])
		m_objectIds.push_back(jobjectId);
}

json Action::getVerbObjectCombo() const
{
	auto j = json::array({m_verbId, {}});
	for (auto &objectId : m_objectIds)
		j[1].push_back(objectId);
	return j;
}

} // namespace NovelTea
