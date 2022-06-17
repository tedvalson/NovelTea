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
		Script,
	};

	enum PageEffect {
		PageEffectNone = 0,
		PageEffectFade = 1,
		PageEffectScrollLeft,
	};

	CutsceneSegment();
	virtual ~CutsceneSegment();

	virtual Type type() const = 0;
	bool conditionPasses() const;

	static std::shared_ptr<CutsceneSegment> createSegment(const json &j);

	virtual size_t getFullDuration() const;
	virtual size_t getFullDelay() const;

	ADD_ACCESSOR(size_t, Duration, m_duration)
	ADD_ACCESSOR(size_t, Delay, m_delay)
	ADD_ACCESSOR(bool, WaitForClick, m_waitForClick)
	ADD_ACCESSOR(bool, CanSkip, m_canSkip)
	ADD_ACCESSOR(std::string, ConditionScript, m_conditionScript)

private:
	size_t m_duration;
	size_t m_delay;
	bool m_waitForClick;
	bool m_canSkip;
	std::string m_conditionScript;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENESEGMENT_HPP
