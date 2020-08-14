#include <NovelTea/DialogueSegment.hpp>

namespace NovelTea
{

DialogueSegment::DialogueSegment()
: m_type (Type::Invalid)
, m_linkId(-1)
, m_conditionalEnabled(false)
, m_scriptedText(false)
, m_scriptEnabled(false)
{
	m_text = "[[nothing]]";
}

bool DialogueSegment::operator==(const DialogueSegment &segment) const
{
	// Excludes childrenIds
	return m_type == segment.m_type &&
			m_linkId == segment.m_linkId &&
			m_conditionalEnabled == segment.m_conditionalEnabled &&
			m_scriptedText == segment.m_scriptedText &&
			m_scriptEnabled == segment.m_scriptEnabled &&
			m_conditionScript == segment.m_conditionScript &&
			m_script == segment.m_script &&
			m_text == segment.m_text;
}

void DialogueSegment::appendChild(int id)
{
	m_childrenIds.push_back(id);
}

void DialogueSegment::clearChildren()
{
	m_childrenIds.clear();
}

json DialogueSegment::toJson() const
{
	auto jchildrenIds = sj::Array();
	for (auto childId : m_childrenIds)
		jchildrenIds.append(childId);

	json j = sj::Array(
		static_cast<int>(m_type),
		m_linkId,
		m_conditionalEnabled,
		m_scriptedText,
		m_scriptEnabled,
		m_conditionScript,
		m_script,
		m_text,
		jchildrenIds
	);
	return j;
}

bool DialogueSegment::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != 9)
		return false;

	m_type = static_cast<Type>(j[0].ToInt());
	m_linkId = j[1].ToInt();
	m_conditionalEnabled = j[2].ToBool();
	m_scriptedText = j[3].ToBool();
	m_scriptEnabled = j[4].ToBool();
	m_conditionScript = j[5].ToString();
	m_script = j[6].ToString();
	m_text = j[7].ToString();
	m_childrenIds.clear();
	for (auto &jchildId : j[8].ArrayRange())
		m_childrenIds.push_back(jchildId.ToInt());

	return true;
}

} // namespace NovelTea
