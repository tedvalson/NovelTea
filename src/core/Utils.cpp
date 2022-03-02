#include <NovelTea/Utils.hpp>

namespace NovelTea
{

std::vector<std::string> split(const std::string &text, const std::string &delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	size_t p = 0;

	while (pos != text.npos)
	{
		pos = text.find(delimiter.c_str(), p, delimiter.size());
		result.push_back(text.substr(p, pos - p));
		p = pos + delimiter.size();
	}
	return result;
}

std::string join(const std::vector<std::string> &words, const std::string &delimiter)
{
	if (words.empty())
		return "";
	std::string result;
	for (auto &word : words)
		result += delimiter + word;
	return result.substr(delimiter.size());
}

bool wrapText(sf::Text &text, float width)
{
	if (text.getLocalBounds().width <= width)
		return false;

	auto s = text.getString().toAnsiString();
	auto words = split(s, " ");
	auto processedWidth = 0.f;
	int pos = 0;
	std::string out;
	sf::Vector2f lastWordPos;
	for (auto &word : words)
	{
		auto p = text.findCharacterPos(pos + word.size());
		if (p.x - processedWidth > width)
		{
			out += "\n" + word + " ";
			pos += word.size() + 1;
			processedWidth += lastWordPos.x - processedWidth;
		} else {
			out += word + " ";
			pos += word.size() + 1;
		}
		lastWordPos = p;
	}

	text.setString(out);
	return true;
}

std::string replace(const std::string &text, const std::string &textToReplace, const std::string &replacement)
{
	// TODO: Optimize
	auto v = split(text, textToReplace);
	return join(v, replacement);
}

EntityType entityIdToType(const std::string &entityId)
{
	if (entityId == ID::Action)
		return EntityType::Action;
	if (entityId == ID::Cutscene)
		return EntityType::Cutscene;
	if (entityId == ID::Dialogue)
		return EntityType::Dialogue;
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
