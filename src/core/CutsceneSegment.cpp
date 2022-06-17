#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutsceneScriptSegment.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Game.hpp>

namespace NovelTea
{

CutsceneSegment::CutsceneSegment()
: m_duration(1000)
, m_delay(1000)
, m_waitForClick(false)
, m_canSkip(true)
{
}

CutsceneSegment::~CutsceneSegment()
{
}

bool CutsceneSegment::conditionPasses() const
{
	if (m_conditionScript.empty())
		return true;
	return ActiveGame->getScriptManager()->runInClosure<bool>(m_conditionScript);
}

std::shared_ptr<CutsceneSegment> CutsceneSegment::createSegment(const json &j)
{
	std::shared_ptr<CutsceneSegment> segment;
	Type type = static_cast<Type>(j[0].ToInt());

	if (type == Type::Text)
		segment.reset(new CutsceneTextSegment);
	else if (type == Type::PageBreak)
		segment.reset(new CutscenePageBreakSegment);
	else if (type == Type::Page)
		segment.reset(new CutscenePageSegment);
	else if (type == Type::Script)
		segment.reset(new CutsceneScriptSegment);
	else
		return nullptr;

	segment->fromJson(j);
	return segment;
}

size_t CutsceneSegment::getFullDuration() const
{
	return m_duration;
}

size_t CutsceneSegment::getFullDelay() const
{
	return m_delay;
}

} // namespace NovelTea
