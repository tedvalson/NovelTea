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
		getScriptOverride(),
		getScriptOverrideName(),
		m_transition,
		getDuration(),
		getDelay(),
		m_beginWithNewline,
		m_activeText->toJson()
	);
	return j;
}

bool CutsceneTextSegment::fromJson(const json &j)
{
	setScriptOverride(j[1].ToBool());
	setScriptOverrideName(j[2].ToString());
	m_transition = j[3].ToInt();
	setDuration(j[4].ToInt());
	setDelay(j[5].ToInt());
	m_beginWithNewline = j[6].ToBool();
	m_activeText = std::make_shared<ActiveText>();
	m_activeText->fromJson(j[7]);
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

void CutsceneTextSegment::setTransition(int transition)
{
	m_transition = transition;
}

int CutsceneTextSegment::getTransition() const
{
	return m_transition;
}

void CutsceneTextSegment::setBeginWithNewLine(bool beginWithNewLine)
{
	m_beginWithNewline = beginWithNewLine;
}

bool CutsceneTextSegment::getBeginWithNewLine() const
{
	return m_beginWithNewline;
}

} // namespace NovelTea
