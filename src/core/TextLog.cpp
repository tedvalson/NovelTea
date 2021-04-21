#include <NovelTea/TextLog.hpp>
#include <SFML/System/Err.hpp>

namespace NovelTea
{

TextLog::TextLog()
: m_itemLimit(100)
{
}

void TextLog::push(const std::string &text, TextLogType type)
{
	m_items.push_back(TextLogItem{text, type});
}

const std::vector<TextLogItem> &TextLog::items() const
{
	return m_items;
}

} // namespace NovelTea
