#include <NovelTea/DialogueSegment.hpp>

namespace NovelTea
{

DialogueSegment::DialogueSegment()
: m_type (Type::Invalid)
{
	m_text = "[[nothing]]";
}

void DialogueSegment::appendChild(int id)
{
	m_childrenIds.push_back(id);
}

json DialogueSegment::toJson() const
{
	auto jchildrenIds = sj::Array();
	for (auto childId : m_childrenIds)
		jchildrenIds.append(childId);

	json j = sj::Array(
		static_cast<int>(m_type),
		m_text,
		jchildrenIds
	);
	return j;
}

bool DialogueSegment::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != 3)
		return false;

	m_type = static_cast<Type>(j[0].ToInt());
	m_text = j[1].ToString();
	m_childrenIds.clear();
	for (auto &jchildId : j[2].ArrayRange())
		m_childrenIds.push_back(jchildId.ToInt());

	return true;
}

std::shared_ptr<DialogueSegment> DialogueSegment::createSegment(const json &j)
{
	std::shared_ptr<DialogueSegment> segment;
	Type type = static_cast<Type>(j[0].ToInt());

//	if (type == Type::Text)
//		segment.reset(new CutsceneTextSegment);
//	else if (type == Type::PageBreak)
//		segment.reset(new CutscenePageBreakSegment);
//	else
		return nullptr;

	segment->fromJson(j);
	return segment;
}

} // namespace NovelTea
