#ifndef NOVELTEA_CUTSCENETEXTSEGMENT_HPP
#define NOVELTEA_CUTSCENETEXTSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/ActiveText.hpp>

namespace NovelTea
{

class CutsceneTextSegment : public CutsceneSegment
{
public:
	CutsceneTextSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	void setActiveText(const std::shared_ptr<ActiveText> &activeText);
	const std::shared_ptr<ActiveText> &getActiveText() const;

	ADD_ACCESSOR(int, OffsetX, m_offsetX)
	ADD_ACCESSOR(int, OffsetY, m_offsetY)
	ADD_ACCESSOR(int, Transition, m_transition)
	ADD_ACCESSOR(bool, BeginWithNewLine, m_beginWithNewline)

private:
	bool m_beginWithNewline = false;
	int m_offsetX = 0;
	int m_offsetY = 0;
	int m_transition = TextEffectFade;
	std::shared_ptr<ActiveText> m_activeText;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENETEXTSEGMENT_HPP
