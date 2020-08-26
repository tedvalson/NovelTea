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
	return 7;
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
		m_defaultName,
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
	m_defaultName = j[3].ToString();
	m_nextEntity = j[4];
	m_rootIndex = j[5].ToInt();
	m_segments.clear();
	for (auto &jsegment : j[6].ArrayRange())
	{
		auto segment = std::make_shared<DialogueSegment>();
		segment->fromJson(jsegment);
		segment->setDialogue(this);
		m_segments.push_back(segment);
	}
}

void Dialogue::clearSegments()
{
	m_segments.clear();
}

void Dialogue::addSegment(std::shared_ptr<DialogueSegment> segment)
{
	segment->setId(m_segments.size());
	m_segments.push_back(segment);
}

std::shared_ptr<DialogueSegment> Dialogue::getSegment(int index, bool followLink) const
{
	auto segment = m_segments[index];
	if (followLink && segment->getType() == DialogueSegment::Link)
		segment = m_segments[segment->getLinkId()];
	return segment;
}

std::vector<std::shared_ptr<DialogueSegment>> &Dialogue::segments()
{
	return m_segments;
}

} // namespace NovelTea
