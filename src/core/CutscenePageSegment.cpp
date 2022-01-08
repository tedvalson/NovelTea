#include <NovelTea/CutscenePageSegment.hpp>

namespace NovelTea
{

CutscenePageSegment::CutscenePageSegment()
: m_textDelay(2000)
, m_textDuration(1000)
, m_breakDelay(3000)
, m_breakDuration(2000)
, m_textEffect(TextEffectFade)
, m_breakEffect(PageEffectFade)
, m_offsetX(0)
, m_offsetY(0)
, m_beginWithNewline(true)
, m_textDelimiter("\n")
, m_breakDelimiter("\n\n")
{
	setWaitForClick(true);
	setCanSkip(true);
}

json CutscenePageSegment::toJson() const
{
	auto j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		getText(),
		getTextDelimiter(),
		getBreakDelimiter(),
		getTextEffect(),
		getBreakEffect(),
		getTextDuration(),
		getTextDelay(),
		getBreakDuration(),
		getBreakDelay(),
		getCanSkip(),
		getBeginWithNewLine(),
		getOffsetX(),
		getOffsetY(),
		getConditionScript()
	);
	return j;
}

bool CutscenePageSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setText(j[2].ToString());
	setTextDelimiter(j[3].ToString());
	setBreakDelimiter(j[4].ToString());
	setTextEffect(j[5].ToInt());
	setBreakEffect(j[6].ToInt());
	setTextDuration(j[7].ToInt());
	setTextDelay(j[8].ToInt());
	setBreakDuration(j[9].ToInt());
	setBreakDelay(j[10].ToInt());
	setCanSkip(j[11].ToBool());
	setBeginWithNewLine(j[12].ToBool());
	setOffsetX(j[13].ToInt());
	setOffsetY(j[14].ToInt());
	setConditionScript(j[15].ToString());
	return true;
}

CutsceneSegment::Type CutscenePageSegment::type() const
{
	return CutsceneSegment::Page;
}

const size_t &CutscenePageSegment::getDuration() const
{
	auto textCount = 0;
	auto pages = split(getText(), getBreakDelimiter());
	for (auto &page : pages)
		textCount += split(page, getTextDelimiter()).size();
	m_duration = m_textDuration * textCount;
	if (textCount > 0 && m_textDelay > m_textDuration)
		m_duration += (m_textDelay - m_textDuration) * (textCount - 1);
	if (!pages.empty())
		m_duration += m_breakDelay * (pages.size() - 1);
	return m_duration;
}

const size_t &CutscenePageSegment::getDelay() const
{
	auto textCount = 0;
	auto pages = split(getText(), getBreakDelimiter());
	for (auto &page : pages)
		textCount += split(page, getTextDelimiter()).size();
	m_delay = m_textDelay * textCount + m_breakDelay * (pages.size() - 1);
	return m_delay;
}

int CutscenePageSegment::getSegmentCount() const
{
	auto pages = split(getText(), getBreakDelimiter());
	auto result = pages.empty() ? 0 : pages.size() - 1;
	for (auto &page : pages)
		result += split(page, getTextDelimiter()).size();
	return result;
}

} // namespace NovelTea
