#ifndef NOVELTEA_DIALOGUESEGMENT_HPP
#define NOVELTEA_DIALOGUESEGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>
#include <vector>

namespace NovelTea
{

class Dialogue;

class DialogueSegment : public JsonSerializable
{
public:
	enum Type {
		Invalid   =-1,
		Root      = 0,
		Text      = 1,
		Option    = 2,
		Link      = 3,
	};

	DialogueSegment();

	bool operator==(const DialogueSegment &segment) const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	void appendChild(int id);
	void clearChildren();

	// If enabled, run script and autosave
	void run();
	bool conditionPasses() const;
	std::string getText(bool *ok = nullptr) const;
	std::vector<std::pair<std::string,std::string>> getTextMultiline(bool *ok = nullptr) const;
	Type getNextType() const;

	bool isEmpty() const;
	bool isComment() const;
	bool isDisabled() const;
	bool isTextNext() const;
	bool isOptionNext() const;

	void setDialogue(Dialogue *dialogue);

	// For scripting compatiblity
	void setTypeInt(int typeInt);
	int getTypeInt() const;

	ADD_ACCESSOR(int, Id, m_id)
	ADD_ACCESSOR(int, LinkId, m_linkId)
	ADD_ACCESSOR(Type, Type, m_type)
	ADD_ACCESSOR(bool, ConditionalEnabled, m_conditionalEnabled)
	ADD_ACCESSOR(bool, ScriptedText, m_scriptedText)
	ADD_ACCESSOR(bool, ScriptEnabled, m_scriptEnabled)
	ADD_ACCESSOR(bool, ShowOnce, m_showOnce)
	ADD_ACCESSOR(bool, Autosave, m_autosave)
	ADD_ACCESSOR(std::string, ConditionScript, m_conditionScript)
	ADD_ACCESSOR(std::string, Script, m_script)
	ADD_ACCESSOR(std::string, TextRaw, m_textRaw)
	ADD_ACCESSOR(std::vector<int>, ChildrenIds, m_childrenIds)

private:
	int m_id;
	int m_linkId;
	Type m_type;
	bool m_conditionalEnabled;
	bool m_scriptedText;
	bool m_scriptEnabled;
	bool m_showOnce;
	bool m_autosave;
	std::string m_conditionScript;
	std::string m_script;
	std::string m_textRaw;
	std::vector<int> m_childrenIds;
	Dialogue *m_dialogue;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUESEGMENT_HPP
