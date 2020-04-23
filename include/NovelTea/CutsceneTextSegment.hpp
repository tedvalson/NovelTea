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

	void setTransition(int transition);
	int getTransition() const;

	void setBeginWithNewLine(bool beginWithNewLine);
	bool getBeginWithNewLine() const;

private:
	bool m_beginWithNewline = false;
	int m_transition = 0;
	std::shared_ptr<ActiveText> m_activeText;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENETEXTSEGMENT_HPP
