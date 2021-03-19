#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>

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
	Type type = static_cast<Type>(j[0].ToInt());

	if (type == Type::Text)
		segment.reset(new CutsceneTextSegment);
	else if (type == Type::PageBreak)
		segment.reset(new CutscenePageBreakSegment);
	else if (type == Type::Page)
		segment.reset(new CutscenePageSegment);
	else
		return nullptr;

	segment->fromJson(j);
	return segment;
}

} // namespace NovelTea
