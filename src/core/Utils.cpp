#include <NovelTea/Utils.hpp>

namespace NovelTea
{

EntityType entityIdToType(const std::string &entityId)
{
	if (entityId == ID::Action)
		return EntityType::Action;
	if (entityId == ID::Cutscene)
		return EntityType::Cutscene;
	if (entityId == ID::Dialogue)
		return EntityType::Dialogue;
	if (entityId == ID::Map)
		return EntityType::Map;
	if (entityId == ID::Object)
		return EntityType::Object;
	if (entityId == ID::Room)
		return EntityType::Room;
	if (entityId == ID::Script)
		return EntityType::Script;
	if (entityId == ID::Verb)
		return EntityType::Verb;
	else
		return EntityType::Invalid;
}

std::string entityTypeToId(EntityType entityType)
{
	if (entityType == EntityType::Action)
		return ID::Action;
	if (entityType == EntityType::Cutscene)
		return ID::Cutscene;
	if (entityType == EntityType::Dialogue)
		return ID::Dialogue;
	if (entityType == EntityType::Map)
		return ID::Map;
	if (entityType == EntityType::Object)
		return ID::Object;
	if (entityType == EntityType::Room)
		return ID::Room;
	if (entityType == EntityType::Script)
		return ID::Script;
	if (entityType == EntityType::Verb)
		return ID::Verb;
	else
		return "";
}

} // namespace NovelTea
