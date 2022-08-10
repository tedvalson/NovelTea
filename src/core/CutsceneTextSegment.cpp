#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/TextTypes.hpp>

namespace NovelTea
{

CutsceneTextSegment::CutsceneTextSegment(Context *context)
: CutsceneSegment(context)
, m_offsetX(0)
, m_offsetY(0)
, m_transition(TextEffect::Fade)
{
	setText("");
}

json CutsceneTextSegment::toJson() const
{
	json j = sj::Array(
		static_cast<int>(type()),
		getWaitForClick(),
		getConditionScript(),
		static_cast<int>(m_transition),
		getDuration(),
		getDelay(),
		getOffsetX(),
		getOffsetY(),
		m_beginWithNewline,
		getCanSkip(),
		m_text
	);
	return j;
}

bool CutsceneTextSegment::fromJson(const json &j)
{
	setWaitForClick(j[1].ToBool());
	setConditionScript(j[2].ToString());
	m_transition = static_cast<TextEffect>(j[3].ToInt());
	setDuration(j[4].ToInt());
	setDelay(j[5].ToInt());
	setOffsetX(j[6].ToInt());
	setOffsetY(j[7].ToInt());
	m_beginWithNewline = j[8].ToBool();
	setCanSkip(j[9].ToBool());
	setText(j[10].ToString());
	return true;
}

CutsceneSegment::Type CutsceneTextSegment::type() const
{
	return CutsceneSegment::Text;
}

const std::shared_ptr<ActiveText> &CutsceneTextSegment::getActiveText() const
{
	return m_activeText;
}

void CutsceneTextSegment::setText(const std::string &text)
{
	m_text = text;

	TextProperties textProps;
	textProps.xOffset = getOffsetX();
	textProps.yOffset = getOffsetY();

	AnimationProperties anim;
	anim.type      = getTransition();
	anim.duration  = getDuration();
	anim.delay     = getDelay();
	anim.skippable = getCanSkip();
	anim.waitForClick = false;
	if (anim.type == TextEffect::FadeAcross)
		anim.equation = &TweenEngine::TweenEquations::easeInOutLinear;
	m_activeText = std::make_shared<ActiveText>(getContext(), m_text, textProps, anim);
}

const std::string &CutsceneTextSegment::getText() const
{
	return m_text;
}

size_t CutsceneTextSegment::getFullDuration() const
{
	return m_activeText->getDurationMs();
}

size_t CutsceneTextSegment::getFullDelay() const
{
	return m_activeText->getDelayMs();
}

} // namespace NovelTea
