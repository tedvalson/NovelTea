#ifndef NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
#define NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>

namespace NovelTea
{

class CutscenePageBreakSegment : public CutsceneSegment
{
public:
	CutscenePageBreakSegment();
	virtual json toJson() const;
	virtual bool fromJson(const json &j);
	virtual Type type() const;

	void setTransition(int transition);
	int getTransition() const;

	void setTransitionDuration(int duration);
	int getTransitionDuration() const;

private:
	int m_transition = 0;
	int m_transitionDuration = 100;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
