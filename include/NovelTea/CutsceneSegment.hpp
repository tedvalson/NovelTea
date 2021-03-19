#ifndef NOVELTEA_CUTSCENESEGMENT_HPP
#define NOVELTEA_CUTSCENESEGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>

namespace NovelTea
{

class CutsceneSegment : public JsonSerializable
{
public:
	enum Type {
		Text      = 0,
		PageBreak = 1,
		Page,
	};

	enum TextEffect {
		TextEffectNone = 0,
		TextEffectFade = 1,
		TextEffectFadeAcross,
	};

	enum PageEffect {
		PageEffectNone = 0,
		PageEffectFade = 1,
		PageEffectScrollLeft,
	};

	CutsceneSegment();
	virtual ~CutsceneSegment();

	virtual Type type() const = 0;

	static std::shared_ptr<CutsceneSegment> createSegment(const json &j);

	ADD_ACCESSOR(size_t, Duration, m_duration)
	ADD_ACCESSOR(size_t, Delay, m_delay)
	ADD_ACCESSOR(bool, WaitForClick, m_waitForClick)
	ADD_ACCESSOR(bool, CanSkip, m_canSkip)

private:
	size_t m_duration = 1000;
	size_t m_delay = 1000;
	bool m_waitForClick = false;
	bool m_canSkip = true;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENESEGMENT_HPP
