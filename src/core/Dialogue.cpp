#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>

namespace NovelTea
{

Dialogue::Dialogue()
: m_rootIndex(0)
, m_nextEntity(sj::Array(-1,""))
{
	auto rootSegment = std::make_shared<DialogueSegment>();
	rootSegment->setType(DialogueSegment::Root);
	m_segments.push_back(rootSegment);
}

size_t Dialogue::jsonSize() const
{
	return 6;
}

json Dialogue::toJson() const
{
	auto jsegments = sj::Array();
	for (auto &seg : m_segments)
		jsegments.append(seg->toJson());

	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_nextEntity,
		m_rootIndex,
		jsegments
	);
	return j;
}

void Dialogue::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_nextEntity = j[3];
	m_rootIndex = j[4].ToInt();
	m_segments.clear();
	for (auto &jsegment : j[5].ArrayRange())
	{
		auto segment = std::make_shared<DialogueSegment>();
		segment->fromJson(jsegment);
		m_segments.push_back(segment);
	}
}

void Dialogue::clearSegments()
{
	m_segments.clear();
}

void Dialogue::addSegment(std::shared_ptr<DialogueSegment> segment)
{
	m_segments.push_back(segment);
}

std::vector<std::shared_ptr<DialogueSegment>> &Dialogue::segments()
{
	return m_segments;
}

} // namespace NovelTea
