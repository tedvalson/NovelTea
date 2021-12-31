#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <iostream>

namespace NovelTea
{

Cutscene::Cutscene()
	: m_fullScreen(true)
	, m_canFastForward(true)
	, m_speedFactor(1.f)
	, m_nextEntity(sj::Array(-1,""))
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
		auto segment = CutsceneSegment::createSegment(jsegment);
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
		auto pageSegment = static_cast<CutscenePageSegment*>(segment.get());
		auto textPages = split(pageSegment->getText(), pageSegment->getBreakDelimiter());
		for (int i = 0; i < textPages.size(); ++i)
		{
			// Process first text segment without page break.
			if (i > 0)
			{
				auto pageBreakSegment = new CutscenePageBreakSegment;
				pageBreakSegment->setTransition(pageSegment->getBreakEffect());
				pageBreakSegment->setDuration(pageSegment->getBreakDuration());
				pageBreakSegment->setDelay(pageSegment->getBreakDelay());
				pageBreakSegment->setWaitForClick(pageSegment->getWaitForClick());
				pageBreakSegment->setCanSkip(pageSegment->getCanSkip());
				m_segments.emplace_back(pageBreakSegment);
			}

			auto texts = split(textPages[i], pageSegment->getTextDelimiter());
			for (int j = 0; j < texts.size(); ++j)
			{
				TextFormat format;
				auto activeText = std::make_shared<ActiveText>();
				activeText->setText(texts[j], format);

				auto textSegment = new CutsceneTextSegment;
				textSegment->setActiveText(activeText);
				textSegment->setBeginWithNewLine(pageSegment->getBeginWithNewLine());
				textSegment->setTransition(pageSegment->getTextEffect());
				textSegment->setDuration(pageSegment->getTextDuration());
				textSegment->setDelay(pageSegment->getTextDelay());
				textSegment->setWaitForClick(pageSegment->getWaitForClick());
				textSegment->setCanSkip(pageSegment->getCanSkip());
				textSegment->setOffsetX(pageSegment->getOffsetX());
				textSegment->setOffsetY(pageSegment->getOffsetY());
				if (j == 0) {
					// Don't wait for click on first seg of new page
					if (i > 0)
						textSegment->setWaitForClick(false);
					// Don't wait for first seg if previous seg was a page break
					else if (m_internalSegments.size() > 1)
						if (m_internalSegments[m_internalSegments.size()-2]->type() == CutsceneSegment::PageBreak)
							textSegment->setWaitForClick(false);
				}
				m_segments.emplace_back(textSegment);
			}
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
		duration += m_internalSegments[i]->getDuration();
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
		delay += m_internalSegments[i]->getDelay();
	return delay;
}

} // namespace NovelTea
