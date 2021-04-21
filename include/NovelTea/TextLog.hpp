#ifndef NOVELTEA_TEXTLOG_HPP
#define NOVELTEA_TEXTLOG_HPP

#include <NovelTea/Utils.hpp>

namespace NovelTea
{

enum class TextLogType {
	Cutscene,
	DialogueOption,
	DialogueText,
	Notification,
	TextOverlay,
};

struct TextLogItem {
	std::string text;
	TextLogType type;
};

class TextLog
{
public:
	TextLog();

	void push(const std::string &text, TextLogType type);
	const std::vector<TextLogItem> &items() const;

	ADD_ACCESSOR(size_t, ItemLimit, m_itemLimit)

private:
	size_t m_itemLimit;
	std::vector<TextLogItem> m_items;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOG_HPP
