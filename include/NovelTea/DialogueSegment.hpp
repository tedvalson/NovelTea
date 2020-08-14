#ifndef NOVELTEA_DIALOGUESEGMENT_HPP
#define NOVELTEA_DIALOGUESEGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>
#include <vector>

namespace NovelTea
{

class DialogueSegment : public JsonSerializable
{
public:
	enum Type {
		Invalid   =-1,
		Root      = 0,
		NPC       = 1,
		Player    = 2,
		Link      = 3,
	};

	DialogueSegment();

	bool operator==(const DialogueSegment &segment) const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	void appendChild(int id);
	void clearChildren();

	ADD_ACCESSOR(Type, Type, m_type)
	ADD_ACCESSOR(bool, ConditionalEnabled, m_conditionalEnabled)
	ADD_ACCESSOR(bool, ScriptedText, m_scriptedText)
	ADD_ACCESSOR(bool, ScriptEnabled, m_scriptEnabled)
	ADD_ACCESSOR(std::string, ConditionScript, m_conditionScript)
	ADD_ACCESSOR(std::string, Script, m_script)
	ADD_ACCESSOR(std::string, Text, m_text)
	ADD_ACCESSOR(std::vector<int>, ChildrenIds, m_childrenIds)

private:
	Type m_type;
	int m_linkId;
	bool m_conditionalEnabled;
	bool m_scriptedText;
	bool m_scriptEnabled;
	std::string m_conditionScript;
	std::string m_script;
	std::string m_text;
	std::vector<int> m_childrenIds;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUESEGMENT_HPP
