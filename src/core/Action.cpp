#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

Action::Action()
: m_positionDependent(false)
{
	
}

size_t Action::jsonSize() const
{
	return 7;
}

json Action::toJson() const
{
	auto jobjects = sj::Array();
	for (auto &objectId : m_objectIds)
		jobjects.append(objectId);
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_verbId,
		m_script,
		jobjects,
		m_positionDependent
	);
	return j;
}

void Action::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_verbId = j[3].ToString();
	m_script = j[4].ToString();
	m_positionDependent = j[6].ToBool();

	m_objectIds.clear();
	for (auto &jobject : j[5].ArrayRange())
		m_objectIds.push_back(jobject.ToString());
}

EntityType Action::entityType() const
{
	return EntityType::Action;
}

void Action::setVerbObjectCombo(const json &j)
{
	if (!j.IsArray() || j.size() != 2 ||
			!j[1].IsArray() || j[1].IsEmpty())
		return;

	auto verb = GSave->get<Verb>(j[0].ToString());
	if (!verb)
		return;

	auto objectCount = verb->getObjectCount();
	if (j[1].size() != objectCount)
		return;

	m_verbId = j[0].ToString();
	m_objectIds.clear();
	for (auto &jobjectId : j[1].ArrayRange())
		m_objectIds.push_back(jobjectId.ToString());
}

json Action::getVerbObjectCombo() const
{
	auto j = sj::Array(m_verbId, sj::Array());
	for (auto &objectId : m_objectIds)
		j[1].append(objectId);
	return j;
}

bool Action::runScript(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	if (m_script.empty())
		return true;
	if (!m_parentId.empty()) {
		auto parentAction = GSave->get<Action>(m_parentId);
		if (parentAction && !parentAction->runScript(verbId, objectIds))
			return false;
	}
	ActiveGame->getScriptManager()->setActiveEntity(GSave->get<Action>(m_id));
	return ActiveGame->getScriptManager()->runActionScript(verbId, objectIds, m_script + "\nreturn true;");
}

bool Action::runScript()
{
	return runScript(m_verbId, m_objectIds);
}

std::shared_ptr<Action> Action::find(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	// TODO: check SaveData
	for (auto &item : ProjData[Action::id].ObjectRange())
	{
		auto j = item.second;
		if (j[3].ToString() == verbId)
		{
			auto match = true;
			auto &jobjects = j[5];
			bool positionDependent = j[6].ToBool();

			if (objectIds.size() != jobjects.size())
				continue;

			for (int i = 0; i < jobjects.size(); ++i)
			{
				if ((positionDependent && objectIds[i] != jobjects[i].ToString()) ||
					(!positionDependent && std::find(objectIds.begin(), objectIds.end(), jobjects[i].ToString()) == objectIds.end()))
				{
					match = false;
					break;
				}
			}

			if (match)
				return GSave->get<Action>(item.first);
		}
	}

	return nullptr;
}

} // namespace NovelTea
