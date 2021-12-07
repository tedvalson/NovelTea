#ifndef NOVELTEA_TEXTLOG_HPP
#define NOVELTEA_TEXTLOG_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

enum class TextLogType {
	Cutscene,
	DialogueOption,
	DialogueText,
	DialogueTextName,
	Notification,
	Script,
	TextOverlay,
};

struct TextLogItem {
	std::string text;
	TextLogType type;
};

class TextLog : public JsonSerializable
{
public:
	TextLog();

	void push(const std::string &text, TextLogType type);
	void pushScript(const std::string &text);
	const std::vector<TextLogItem> &items() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	ADD_ACCESSOR(size_t, ItemLimit, m_itemLimit)

private:
	size_t m_itemLimit;
	std::vector<TextLogItem> m_items;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOG_HPP
