#include <NovelTea/CutsceneTextSegment.hpp>

namespace NovelTea
{

CutsceneTextSegment::CutsceneTextSegment()
{
	m_activeText = std::make_shared<ActiveText>();
}

json CutsceneTextSegment::toJson() const
{
	json j = json::array({
		type(),
		getScriptOverride(),
		getScriptOverrideName(),
		m_transition,
		getDuration(),
		m_beginWithNewline,
		*m_activeText
	});
	return j;
}

bool CutsceneTextSegment::fromJson(const json &j)
{
	setScriptOverride(j[1]);
	setScriptOverrideName(j[2]);
	m_transition = j[3];
	setDuration(j[4]);
	m_beginWithNewline = j[5];
	m_activeText = std::make_shared<ActiveText>(j[6]);
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
