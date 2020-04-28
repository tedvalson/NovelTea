#include <NovelTea/CutscenePageBreakSegment.hpp>

namespace NovelTea
{

CutscenePageBreakSegment::CutscenePageBreakSegment()
{
}

json CutscenePageBreakSegment::toJson() const
{
	json j = json::array({
		type(),
		getScriptOverride(),
		getScriptOverrideName(),
		m_transition,
		getDuration(),
		getDelay()
	});
	return j;
}

bool CutscenePageBreakSegment::fromJson(const json &j)
{
	setScriptOverride(j[1]);
	setScriptOverrideName(j[2]);
	m_transition = j[3];
	setDuration(j[4]);
	setDelay(j[5]);
	return true;
}

CutsceneSegment::Type CutscenePageBreakSegment::type() const
{
	return CutsceneSegment::PageBreak;
}

void CutscenePageBreakSegment::setTransition(int transition)
{
	m_transition = transition;
}

int CutscenePageBreakSegment::getTransition() const
{
	return m_transition;
}

} // namespace NovelTea
