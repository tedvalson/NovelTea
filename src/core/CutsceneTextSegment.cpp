#include <NovelTea/CutsceneTextSegment.hpp>

namespace NovelTea
{

CutsceneTextSegment::CutsceneTextSegment()
{
}

json CutsceneTextSegment::toJson() const
{
	json j = json::array({
		type(),
		getScriptOverride(),
		getScriptOverrideName(),
		m_transition,
		m_transitionDuration,
		m_beginWithNewline,
		m_activeText
	});
	return j;
}

bool CutsceneTextSegment::fromJson(const json &j)
{
	setScriptOverride(j[1]);
	setScriptOverrideName(j[2]);
	m_transition = j[3];
	m_transitionDuration = j[4];
	m_beginWithNewline = j[5];
	m_activeText = j[6];
	return true;
}

CutsceneSegment::Type CutsceneTextSegment::type() const
{
	return CutsceneSegment::Text;
}

void CutsceneTextSegment::setActiveText(const ActiveText &activeText)
{
	m_activeText = activeText;
}

const ActiveText &CutsceneTextSegment::getActiveText() const
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

void CutsceneTextSegment::setTransitionDuration(int duration)
{
	m_transitionDuration = duration;
}

int CutsceneTextSegment::getTransitionDuration() const
{
	return m_transitionDuration;
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
