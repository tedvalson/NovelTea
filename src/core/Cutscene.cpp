#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <iostream>

namespace NovelTea
{

Cutscene::Cutscene()
	: m_fullScreen(true)
	, m_canFastForward(true)
	, m_speedFactor(1.f)
	, m_nextEntity(json::array({-1,""}))
{
}

Cutscene::~Cutscene()
{
	std::cout << "cutscene destroyed!" << std::endl;
}

json Cutscene::toJson() const
{
	auto jsegments = json::array();
	for (auto &seg : m_segments)
		jsegments.push_back(*seg);

	auto j = json::array({
		m_fullScreen,
		m_canFastForward,
		m_speedFactor,
		m_nextEntity,
		jsegments,
	});
	return j;
}

bool Cutscene::fromJson(const json &j)
{
	m_segments.clear();
	m_fullScreen = j[0];
	m_canFastForward = j[1];
	m_speedFactor = j[2];
	m_nextEntity = j[3];
	for (auto &jsegment : j[4])
	{
		auto segment = CutsceneSegment::createSegment(jsegment);
		if (segment)
			m_segments.push_back(segment);
		else
			std::cout << "unknown segment type!" << std::endl;
	}
	return true;
}

void Cutscene::addSegment(std::shared_ptr<CutsceneSegment> segment)
{
	m_segments.push_back(segment);
}

std::vector<std::shared_ptr<CutsceneSegment>> &Cutscene::segments()
{
	return m_segments;
}

size_t Cutscene::getDurationMs() const
{
	if (m_segments.empty())
		return 0;
	else
		return getDurationMs(m_segments.size());
}

size_t Cutscene::getDurationMs(size_t indexEnd) const
{
	auto duration = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		duration += m_segments[i]->getDuration();
	return duration;
}

size_t Cutscene::getDelayMs() const
{
	if (m_segments.empty())
		return 0;
	else
		return getDelayMs(m_segments.size());
}

size_t Cutscene::getDelayMs(size_t indexEnd) const
{
	auto delay = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		delay += m_segments[i]->getDelay();
	return delay;
}

} // namespace NovelTea
