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

size_t CutsceneTextSegment::getFullDuration() const
{
	std::cout << "CutsceneTextSegment::getFullDuration()" << std::endl;
	size_t duration = 0;
	auto s = BBCodeParser::makeSegments(getText(), getTextProps(), getAnimProps());
	for (auto &ss : s) {
		if (ss->newGroup) {
			duration += ss->anim.duration / ss->anim.speed;
//			auto d = ss->anim.duration;
			// TODO: Figure out howto calculate length
//			if (ss->anim.type == TextEffect::FadeAcross && duration <= 0)
//				d = 1000.f * getFadeAcrossLength() / 410.f / GConfig.dpiMultiplier; // TODO: Change based on font multiplier
		}
	}
	return duration;
}

size_t CutsceneTextSegment::getFullDelay() const
{
	size_t delay = 0;
	auto s = BBCodeParser::makeSegments(getText(), getTextProps(), getAnimProps());
	for (auto &ss : s) {
		if (ss->newGroup) {
			delay += ss->anim.delay / ss->anim.speed;
//			duration += ss->anim.duration / ss->anim.speed;
//			if (anim.type == TextEffect::FadeAcross && delay < 0)
//				return (ss->anim.duration / ss->anim.speed) - ss->anim.delay;
//			return ss->anim.delay / ss->anim.speed;
		}
	}
	return delay;
}

TextProperties CutsceneTextSegment::getTextProps() const
{
	TextProperties textProps;
	textProps.xOffset = getOffsetX();
	textProps.yOffset = getOffsetY();
	return std::move(textProps);
}

AnimationProperties CutsceneTextSegment::getAnimProps() const
{
	AnimationProperties animProps;
	animProps.type      = getTransition();
	animProps.duration  = getDuration();
	animProps.delay     = getDelay();
	animProps.skippable = getCanSkip();
	animProps.waitForClick = false;
	if (animProps.type == TextEffect::FadeAcross)
		animProps.equation = &TweenEngine::TweenEquations::easeInOutLinear;
	return std::move(animProps);
}

} // namespace NovelTea
