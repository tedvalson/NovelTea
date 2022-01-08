#include <NovelTea/CutsceneScriptSegment.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

CutsceneScriptSegment::CutsceneScriptSegment()
: m_autosaveAfter(false)
, m_autosaveBefore(false)
{
	setDuration(10);
	setDelay(10);
}

json CutsceneScriptSegment::toJson() const
{
	auto j = sj::Array(
		static_cast<int>(type()),
		m_autosaveAfter,
		m_autosaveBefore,
		m_comment,
		m_script,
		getConditionScript()
	);
	return j;
}

bool CutsceneScriptSegment::fromJson(const json &j)
{
	m_autosaveAfter = j[1].ToBool();
	m_autosaveBefore = j[2].ToBool();
	m_comment = j[3].ToString();
	m_script = j[4].ToString();
	setConditionScript(j[5].ToString());
	return true;
}

CutsceneSegment::Type CutsceneScriptSegment::type() const
{
	return CutsceneSegment::Script;
}

void CutsceneScriptSegment::runScript(const std::shared_ptr<Cutscene> &cutscene) const
{
	if (m_script.empty())
		return;
	try {
		ActiveGame->getScriptManager()->setActiveEntity(cutscene);
		ActiveGame->getScriptManager()->runInClosure(m_script);
	} catch (std::exception &e) {
		std::cerr << "CutsceneScriptSegment::runScript() " << e.what() << std::endl;
	}
}

} // namespace NovelTea
