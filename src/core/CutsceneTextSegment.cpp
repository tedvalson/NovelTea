#include <NovelTea/CutsceneTextSegment.hpp>

namespace NovelTea
{

CutsceneTextSegment::CutsceneTextSegment()
{
	m_activeText = std::make_shared<ActiveText>();
}

json CutsceneTextSegment::toJson() const
{
	json j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		getScriptOverrideName(),
		m_transition,
		getDuration(),
		getDelay(),
		getOffsetX(),
		getOffsetY(),
		m_beginWithNewline,
		getCanSkip(),
		m_activeText->toJson()
	);
	return j;
}

bool CutsceneTextSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setScriptOverrideName(j[2].ToString());
	m_transition = j[3].ToInt();
	setDuration(j[4].ToInt());
	setDelay(j[5].ToInt());
	setOffsetX(j[6].ToInt());
	setOffsetY(j[7].ToInt());
	m_beginWithNewline = j[8].ToBool();
	setCanSkip(j[9].ToBool());
	m_activeText = std::make_shared<ActiveText>();
	m_activeText->fromJson(j[10]);
	return true;
}

CutsceneSegment::Type CutsceneTextSegment::type() const
{
	return CutsceneSegment::Text;
}

void CutsceneTextSegment::setActiveText(const std::shared_ptr<ActiveText> &activeText)
{
	m_activeText = activeText;
}

const std::shared_ptr<ActiveText> &CutsceneTextSegment::getActiveText() const
{
	return m_activeText;
}

} // namespace NovelTea
