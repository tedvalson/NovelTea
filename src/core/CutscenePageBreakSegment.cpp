#include <NovelTea/CutscenePageBreakSegment.hpp>

namespace NovelTea
{

CutscenePageBreakSegment::CutscenePageBreakSegment()
{
	setWaitForClick(false);
	setCanSkip(false);
}

json CutscenePageBreakSegment::toJson() const
{
	auto j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		getConditionScript(),
		m_transition,
		getDuration(),
		getDelay(),
		getCanSkip()
	);
	return j;
}

bool CutscenePageBreakSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setConditionScript(j[2].ToString());
	m_transition = j[3].ToInt();
	setDuration(j[4].ToInt());
	setDelay(j[5].ToInt());
	setCanSkip(j[6].ToBool());
	return true;
}

CutsceneSegment::Type CutscenePageBreakSegment::type() const
{
	return CutsceneSegment::PageBreak;
}

} // namespace NovelTea
