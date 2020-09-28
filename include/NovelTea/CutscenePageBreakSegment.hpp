#ifndef NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
#define NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class CutscenePageBreakSegment : public CutsceneSegment
{
public:
	enum TransitionEffect {
		None = 0,
		Fade = 1,
		ScrollLeft,
	};

	CutscenePageBreakSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	ADD_ACCESSOR(int, Transition, m_transition)

private:
	int m_transition = Fade;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENEPAGEBREAKSEGMENT_HPP
