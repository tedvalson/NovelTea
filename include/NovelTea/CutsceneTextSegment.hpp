#ifndef NOVELTEA_CUTSCENETEXTSEGMENT_HPP
#define NOVELTEA_CUTSCENETEXTSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/BBCodeParser.hpp>

namespace NovelTea
{

class CutsceneTextSegment : public CutsceneSegment
{
public:
	CutsceneTextSegment(Context *context);
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	size_t getFullDuration() const override;
	size_t getFullDelay() const override;

	TextProperties getTextProps() const;
	AnimationProperties getAnimProps() const;

	ADD_ACCESSOR(int, OffsetX, m_offsetX)
	ADD_ACCESSOR(int, OffsetY, m_offsetY)
	ADD_ACCESSOR(TextEffect, Transition, m_transition)
	ADD_ACCESSOR(bool, BeginWithNewLine, m_beginWithNewline)
	ADD_ACCESSOR(std::string, Text, m_text)

private:
	bool m_beginWithNewline;
	int m_offsetX;
	int m_offsetY;
	TextEffect m_transition;
	std::string m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENETEXTSEGMENT_HPP
