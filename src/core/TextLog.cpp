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

void TextLog::pushScript(const std::string &text)
{
	push(text, TextLogType::Script);
}

const std::vector<TextLogItem> &TextLog::items() const
{
	return m_items;
}

json TextLog::toJson() const
{
	auto j = sj::Array();
	for (auto &item : m_items)
		j.append(sj::Array(item.text, static_cast<int>(item.type)));
	return j;
}

bool TextLog::fromJson(const json &j)
{
	m_items.clear();
	for (auto &jitem : j.ArrayRange())
		push(jitem[0].ToString(), static_cast<TextLogType>(jitem[1].ToInt()));
	return true;
}

} // namespace NovelTea
