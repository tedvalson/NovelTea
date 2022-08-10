#ifndef NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
#define NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class CutscenePageBreakSegment : public CutsceneSegment
{
public:
	CutscenePageBreakSegment(Context *context);
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	ADD_ACCESSOR(int, Transition, m_transition)

private:
	int m_transition = PageEffectFade;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
