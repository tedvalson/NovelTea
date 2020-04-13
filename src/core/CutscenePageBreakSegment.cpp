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
		m_transitionDuration
	});
	return j;
}

bool CutscenePageBreakSegment::fromJson(const json &j)
{
	setScriptOverride(j[1]);
	setScriptOverrideName(j[2]);
	m_transition = j[3];
	m_transitionDuration = j[4];
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

void CutscenePageBreakSegment::setTransitionDuration(int duration)
{
	m_transitionDuration = duration;
}

int CutscenePageBreakSegment::getTransitionDuration() const
{
	return m_transitionDuration;
}

} // namespace NovelTea
