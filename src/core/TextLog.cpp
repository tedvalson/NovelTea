#include <NovelTea/TextLog.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/BBCodeParser.hpp>
#include <NovelTea/Err.hpp>
#include <NovelTea/Event.hpp>

namespace NovelTea
{

TextLog::TextLog(Context* context)
: Subsystem(context)
, m_itemLimit(5000)
{
}

TextLog::~TextLog()
{
}

void TextLog::push(const std::string &text, TextLogType type)
{
	// TODO: prune old items
		m_entries.push_back({BBCodeParser::stripTags(text), type});
	Event event(Event::TextLogged);
	*event.textlog = {text, type};
	EventMan->push(event);
}

void TextLog::pushScript(const std::string &text)
{
	push(text, TextLogType::Script);
}

const std::vector<TextLogEntry> &TextLog::entries() const
{
	return m_entries;
}

json TextLog::toJson() const
{
	auto j = sj::Array();
	int count = std::min(m_entries.size(), m_itemLimit);
	for (int i = m_entries.size() - count; i < m_entries.size(); ++i) {
		auto& entry = m_entries[i];
		j.append(sj::Array(entry.text, static_cast<int>(entry.type)));
	}
	return j;
}

bool TextLog::fromJson(const json &j)
{
	m_entries.clear();
	for (auto &jentry : j.ArrayRange())
		push(jentry[0].ToString(), static_cast<TextLogType>(jentry[1].ToInt()));
	return true;
}

} // namespace NovelTea
