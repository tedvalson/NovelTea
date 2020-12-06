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
	};

	CutsceneSegment();
	virtual ~CutsceneSegment();

	virtual Type type() const = 0;

	static std::shared_ptr<CutsceneSegment> createSegment(const json &j);

	ADD_ACCESSOR(size_t, Duration, m_duration)
	ADD_ACCESSOR(size_t, Delay, m_delay)
	ADD_ACCESSOR(bool, WaitForClick, m_waitForClick)
	ADD_ACCESSOR(bool, CanSkip, m_canSkip)
	ADD_ACCESSOR(std::string, ScriptOverrideName, m_scriptOverrideName)

private:
	size_t m_duration = 1000;
	size_t m_delay = 1000;
	bool m_waitForClick = false;
	bool m_canSkip = true;
	std::string m_scriptOverrideName;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENESEGMENT_HPP
