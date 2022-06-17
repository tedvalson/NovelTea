#ifndef NOVELTEA_CUTSCENETEXTSEGMENT_HPP
#define NOVELTEA_CUTSCENETEXTSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextTypes.hpp>

namespace NovelTea
{

class CutsceneTextSegment : public CutsceneSegment
{
public:
	CutsceneTextSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	const std::shared_ptr<ActiveText> &getActiveText() const;
	void setText(const std::string &text);
	const std::string &getText() const;

	size_t getFullDuration() const override;
	size_t getFullDelay() const override;

	ADD_ACCESSOR(int, OffsetX, m_offsetX)
	ADD_ACCESSOR(int, OffsetY, m_offsetY)
	ADD_ACCESSOR(TextEffect, Transition, m_transition)
	ADD_ACCESSOR(bool, BeginWithNewLine, m_beginWithNewline)

private:
	bool m_beginWithNewline = false;
	int m_offsetX = 0;
	int m_offsetY = 0;
	TextEffect m_transition = TextEffect::Fade;
	std::shared_ptr<ActiveText> m_activeText;
	std::string m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENETEXTSEGMENT_HPP
