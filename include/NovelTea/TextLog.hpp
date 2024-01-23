#ifndef NOVELTEA_TEXTLOG_HPP
#define NOVELTEA_TEXTLOG_HPP

#include <NovelTea/Subsystem.hpp>
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

struct TextLogEntry {
	std::string text;
	TextLogType type;
};

class TextLog : public Subsystem, public JsonSerializable
{
public:
	TextLog(Context* context);
	virtual ~TextLog();

	static std::string name() { return "TextLog"; }

	virtual void push(const std::string &text, TextLogType type);
	virtual void pushScript(const std::string &text);
	const std::vector<TextLogEntry> &entries() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	ADD_ACCESSOR(size_t, ItemLimit, m_itemLimit)

protected:
	std::vector<TextLogEntry> m_entries;

private:
	size_t m_itemLimit;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOG_HPP
