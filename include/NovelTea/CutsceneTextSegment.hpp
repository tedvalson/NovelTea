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
	virtual json toJson() const;
	virtual bool fromJson(const json &j);
	virtual Type type() const;

	void setActiveText(const ActiveText &activeText);
	const ActiveText &getActiveText() const;

	void setTransition(int transition);
	int getTransition() const;

	void setTransitionDuration(int duration);
	int getTransitionDuration() const;

	void setBeginWithNewLine(bool beginWithNewLine);
	bool getBeginWithNewLine() const;

private:
	bool m_beginWithNewline = false;
	int m_transition = 0;
	int m_transitionDuration = 100;
	ActiveText m_activeText;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENETEXTSEGMENT_HPP
