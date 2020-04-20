#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>

namespace NovelTea
{

CutsceneSegment::CutsceneSegment()
{
}

CutsceneSegment::~CutsceneSegment()
{
}

std::shared_ptr<CutsceneSegment> CutsceneSegment::createSegment(const json &j)
{
	std::shared_ptr<CutsceneSegment> segment;
	Type type = j[0];

	if (type == Type::Text)
		segment.reset(new CutsceneTextSegment);
	else if (type == Type::PageBreak)
		segment.reset(new CutscenePageBreakSegment);
	else
		return nullptr;

	segment->fromJson(j);
	return segment;
}

void CutsceneSegment::setScriptOverride(bool override)
{
	m_scriptOverride = override;
}

bool CutsceneSegment::getScriptOverride() const
{
	return m_scriptOverride;
}

void CutsceneSegment::setScriptOverrideName(const std::string &scriptName)
{
	m_scriptOverrideName = scriptName;
}

const std::string &CutsceneSegment::getScriptOverrideName() const
{
	return m_scriptOverrideName;
}

} // namespace NovelTea
