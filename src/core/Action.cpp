#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

namespace NovelTea
{

Action::Action()
: m_positionDependent(false)
, m_properties(json::object())
{
	
}

json Action::toJson() const
{
	auto j = json::array({
		m_id,
		m_verbId,
		m_script,
		m_objectIds,
		m_positionDependent,
		m_properties,
	});
	return j;
}

bool Action::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 6)
		return false;

	try
	{
		m_id = j[0];
		m_verbId = j[1];
		m_script = j[2];
		m_positionDependent = j[4];
		m_properties = j[5];

		m_objectIds.clear();
		for (auto &jobject : j[3])
			m_objectIds.push_back(jobject);

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

	auto verb = Save.get<Verb>(j[0]);
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

std::shared_ptr<Action> Action::find(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	// TODO: check SaveData
	for (auto &item : ProjData[Action::id].items())
	{
		auto j = item.value();
		if (j[1] == verbId)
		{
			auto match = true;
			auto &jobjects = j[3];
			bool positionDependent = j[4];

			if (objectIds.size() != jobjects.size())
				continue;

			for (int i = 0; i < jobjects.size(); ++i)
			{
				if ((positionDependent && objectIds[i] != jobjects[i]) ||
					(!positionDependent && std::find(objectIds.begin(), objectIds.end(), jobjects[i]) == objectIds.end()))
				{
					match = false;
					break;
				}
			}

			if (match)
				return Save.get<Action>(item.key());
		}
	}

	return nullptr;
}

} // namespace NovelTea
