#ifndef NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
#define NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>

namespace NovelTea
{

class CutscenePageBreakSegment : public CutsceneSegment
{
public:
	CutscenePageBreakSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	void setTransition(int transition);
	int getTransition() const;

private:
	int m_transition = 0;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
