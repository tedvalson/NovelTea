#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/StringUtils.hpp>

namespace NovelTea
{

CutscenePageSegment::CutscenePageSegment(Context *context)
: CutsceneSegment(context)
, m_textDelay(2000)
, m_textDuration(1000)
, m_breakDelay(3000)
, m_breakDuration(2000)
, m_textEffect(TextEffect::Fade)
, m_breakEffect(PageEffectFade)
, m_offsetX(0)
, m_offsetY(0)
, m_beginWithNewline(true)
, m_endWithPageBreak(true)
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
		static_cast<int>(getTextEffect()),
		getBreakEffect(),
		getTextDuration(),
		getTextDelay(),
		getBreakDuration(),
		getBreakDelay(),
		getCanSkip(),
		getBeginWithNewLine(),
		getOffsetX(),
		getOffsetY(),
		getConditionScript(),
		getEndWithPageBreak()
	);
	return j;
}

bool CutscenePageSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setText(j[2].ToString());
	setTextDelimiter(j[3].ToString());
	setBreakDelimiter(j[4].ToString());
	setTextEffect(static_cast<TextEffect>(j[5].ToInt()));
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
	setEndWithPageBreak(j[16].ToBool());
	buildSegments();
	return true;
}

CutsceneSegment::Type CutscenePageSegment::type() const
{
	return CutsceneSegment::Page;
}

size_t CutscenePageSegment::getFullDuration() const
{
	return m_duration;
}

size_t CutscenePageSegment::getFullDelay() const
{
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

const std::vector<std::shared_ptr<CutsceneSegment> > &CutscenePageSegment::getSegments() const
{
	return m_segments;
}

void CutscenePageSegment::buildSegments()
{
	m_segments.clear();
	m_duration = 0;
	m_delay = 0;

	auto pushPageBreak = [this](){
		auto pageBreakSegment = new CutscenePageBreakSegment(getContext());
		pageBreakSegment->setTransition(getBreakEffect());
		pageBreakSegment->setDuration(getBreakDuration());
		pageBreakSegment->setDelay(getBreakDelay());
		pageBreakSegment->setWaitForClick(false);
		pageBreakSegment->setCanSkip(getCanSkip());
		m_duration += getBreakDuration();
		m_delay += getBreakDelay();
		m_segments.emplace_back(pageBreakSegment);
	};

	auto textPages = split(getText(), getBreakDelimiter());
	for (int i = 0; i < textPages.size(); ++i)
	{
		// Process first text segment without page break.
		if (i > 0)
			pushPageBreak();

		auto texts = split(textPages[i], getTextDelimiter());
		for (auto& text : texts)
		{
			auto textSegment = new CutsceneTextSegment(getContext());
			textSegment->setBeginWithNewLine(getBeginWithNewLine());
			textSegment->setTransition(getTextEffect());
			textSegment->setDuration(getTextDuration());
			textSegment->setDelay(getTextDelay());
			textSegment->setWaitForClick(getWaitForClick());
			textSegment->setCanSkip(getCanSkip());
			textSegment->setOffsetX(getOffsetX());
			textSegment->setOffsetY(getOffsetY());
			textSegment->setText(text); // TODO: Make it so this doesn't NEED to be set last
			m_duration += textSegment->getFullDuration();
			m_delay += textSegment->getFullDelay();
			m_segments.emplace_back(textSegment);
		}
	}

	if (getEndWithPageBreak())
		pushPageBreak();
}

} // namespace NovelTea
