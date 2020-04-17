#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <iostream>

namespace NovelTea
{

Cutscene::Cutscene()
	: m_name("New Cutscene")
{
}

Cutscene::~Cutscene()
{
	std::cout << "cutscene destroyed!" << std::endl;
}

json Cutscene::toJson() const
{
	json j = json::array();
	for (auto &seg : m_segments)
		j.push_back(*seg);
	return j;
}

bool Cutscene::fromJson(const json &j)
{
	m_segments.clear();
	for (auto &jsegment : j)
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

const std::vector<std::shared_ptr<CutsceneSegment>> &Cutscene::segments() const
{
	return m_segments;
}

} // namespace NovelTea
