#ifndef NOVELTEA_CUTSCENEPAGESEGMENT_HPP
#define NOVELTEA_CUTSCENEPAGESEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class CutscenePageSegment : public CutsceneSegment
{
public:
	CutscenePageSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	const size_t &getDuration() const override;
	const size_t &getDelay() const override;

	int getSegmentCount() const;

	ADD_ACCESSOR(size_t, TextDelay, m_textDelay)
	ADD_ACCESSOR(size_t, TextDuration, m_textDuration)
	ADD_ACCESSOR(size_t, BreakDelay, m_breakDelay)
	ADD_ACCESSOR(size_t, BreakDuration, m_breakDuration)
	ADD_ACCESSOR(int, TextEffect, m_textEffect)
	ADD_ACCESSOR(int, BreakEffect, m_breakEffect)
	ADD_ACCESSOR(std::string, TextDelimiter, m_textDelimiter)
	ADD_ACCESSOR(std::string, BreakDelimiter, m_breakDelimiter)
	ADD_ACCESSOR(std::string, Text, m_text)
	ADD_ACCESSOR(int, OffsetX, m_offsetX)
	ADD_ACCESSOR(int, OffsetY, m_offsetY)
	ADD_ACCESSOR(bool, BeginWithNewLine, m_beginWithNewline)

private:
	mutable size_t m_delay;
	mutable size_t m_duration;
	size_t m_textDelay;
	size_t m_textDuration;
	size_t m_breakDelay;
	size_t m_breakDuration;
	int m_textEffect;
	int m_breakEffect;
	int m_offsetX;
	int m_offsetY;
	bool m_beginWithNewline;
	std::string m_textDelimiter;
	std::string m_breakDelimiter;
	std::string m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENEPAGESEGMENT_HPP
