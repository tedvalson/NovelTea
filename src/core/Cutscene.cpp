#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/StringUtils.hpp>
#include <iostream>

namespace NovelTea
{

Cutscene::Cutscene(Context *context)
	: Entity(context)
	, m_fullScreen(true)
	, m_canFastForward(true)
	, m_speedFactor(1.f)
	, m_nextEntity(sj::Array(-1,""))
	, m_skipConditionChecks(false)
{
}

Cutscene::~Cutscene()
{
}

size_t Cutscene::jsonSize() const
{
	return 8;
}

json Cutscene::toJson() const
{
	auto jsegments = sj::Array();
	for (auto &seg : m_internalSegments)
		jsegments.append(seg->toJson());

	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_fullScreen,
		m_canFastForward,
		m_speedFactor,
		m_nextEntity,
		jsegments
	);
	return j;
}

void Cutscene::loadJson(const json &j)
{
	m_segments.clear();
	m_internalSegments.clear();
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_fullScreen = j[3].ToBool();
	m_canFastForward = j[4].ToBool();
	m_speedFactor = j[5].ToFloat();
	m_nextEntity = j[6];
	for (auto &jsegment : j[7].ArrayRange())
	{
		auto segment = CutsceneSegment::createSegment(getContext(), jsegment);
		if (segment)
			addSegment(segment);
		else
			std::cout << "unknown segment type!" << std::endl;
	}
}

EntityType Cutscene::entityType() const
{
	return EntityType::Cutscene;
}

void Cutscene::addSegment(std::shared_ptr<CutsceneSegment> segment)
{
	m_internalSegments.push_back(segment);
	if (segment->type() == CutsceneSegment::Page)
	{
		auto firstProcessed = false;
		auto pageSegment = static_cast<CutscenePageSegment*>(segment.get());
		for (auto &seg : pageSegment->getSegments())
		{
			if (!firstProcessed && seg->type() == CutsceneSegment::Text) {
				firstProcessed = true;

				// Don't wait for first seg if previous seg was a page break or script
				if (m_internalSegments.size() > 1) {
					auto prevSegmentType = m_internalSegments[m_internalSegments.size()-2]->type();
				}
			}
			m_segments.push_back(seg);
		}
	}
	else
		m_segments.push_back(segment);
}

void Cutscene::updateSegments()
{
	auto internalSegments = m_internalSegments;
	m_internalSegments.clear();
	m_segments.clear();
	for (auto &segment : internalSegments)
		addSegment(segment);
}

std::vector<std::shared_ptr<CutsceneSegment>> &Cutscene::segments()
{
	return m_segments;
}

std::vector<std::shared_ptr<CutsceneSegment>> &Cutscene::internalSegments()
{
	return m_internalSegments;
}

size_t Cutscene::getDurationMs() const
{
	if (m_internalSegments.empty())
		return 0;
	else
		return getDurationMs(m_internalSegments.size());
}

size_t Cutscene::getDurationMs(size_t indexEnd) const
{
	auto duration = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		if (m_skipConditionChecks || m_internalSegments[i]->conditionPasses())
			duration += m_internalSegments[i]->getFullDuration();
	return duration;
}

size_t Cutscene::getDelayMs() const
{
	if (m_internalSegments.empty())
		return 0;
	else
		return getDelayMs(m_internalSegments.size());
}

size_t Cutscene::getDelayMs(size_t indexEnd) const
{
	auto delay = 0u;
	for (auto i = 0u; i < indexEnd; ++i)
		if (m_skipConditionChecks || m_internalSegments[i]->conditionPasses())
			delay += m_internalSegments[i]->getFullDelay();
	return delay;
}

void Cutscene::setNextEntity(std::shared_ptr<Entity> entity)
{
	m_nextEntity = sj::Array(static_cast<int>(entity->entityType()), entity->getId());
}

std::shared_ptr<Entity> Cutscene::getNextEntity() const
{
	return Entity::fromEntityJson(getContext(), m_nextEntity);
}

} // namespace NovelTea
