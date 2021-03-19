#include <NovelTea/CutscenePageBreakSegment.hpp>

namespace NovelTea
{

CutscenePageBreakSegment::CutscenePageBreakSegment()
{
	setWaitForClick(true);
	setCanSkip(false);
}

json CutscenePageBreakSegment::toJson() const
{
	auto j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		"",
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
