#include <NovelTea/CutscenePageBreakSegment.hpp>

namespace NovelTea
{

CutscenePageBreakSegment::CutscenePageBreakSegment()
{
}

json CutscenePageBreakSegment::toJson() const
{
	auto j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		getScriptOverrideName(),
		m_transition,
		getDuration(),
		getDelay()
	);
	return j;
}

bool CutscenePageBreakSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setScriptOverrideName(j[2].ToString());
	m_transition = j[3].ToInt();
	setDuration(j[4].ToInt());
	setDelay(j[5].ToInt());
	return true;
}

CutsceneSegment::Type CutscenePageBreakSegment::type() const
{
	return CutsceneSegment::PageBreak;
}

} // namespace NovelTea
