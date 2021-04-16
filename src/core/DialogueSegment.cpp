#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

DialogueSegment::DialogueSegment()
: m_id(-1)
, m_linkId(-1)
, m_type(Type::Invalid)
, m_conditionalEnabled(false)
, m_scriptedText(false)
, m_scriptEnabled(false)
, m_showOnce(false)
, m_dialogue(nullptr)
{

}

bool DialogueSegment::operator==(const DialogueSegment &segment) const
{
	// Excludes id and childrenIds
	return m_type == segment.m_type &&
			m_linkId == segment.m_linkId &&
			m_conditionalEnabled == segment.m_conditionalEnabled &&
			m_scriptedText == segment.m_scriptedText &&
			m_scriptEnabled == segment.m_scriptEnabled &&
			m_showOnce == segment.m_showOnce &&
			m_conditionScript == segment.m_conditionScript &&
			m_script == segment.m_script &&
			m_textRaw == segment.m_textRaw;
}

void DialogueSegment::appendChild(int id)
{
	m_childrenIds.push_back(id);
}

void DialogueSegment::clearChildren()
{
	m_childrenIds.clear();
}

void DialogueSegment::runScript()
{
	if (!m_scriptEnabled)
		return;
	try {
		auto dialogue = GSave->get<Dialogue>(m_dialogue->getId());
		auto script = "function _f(dialogue){\n" + m_script + "}";
		ActiveGame->getScriptManager()->call(script, "_f", dialogue);
	} catch (std::exception &e) {
		std::cerr << "DialogueSegment::runScript() " << e.what() << std::endl;
	}
}

bool DialogueSegment::conditionPasses() const
{
	if (!m_conditionalEnabled || m_conditionScript.empty())
		return true;
	else if (!m_dialogue)
		return false;

	try {
		auto dialogue = GSave->get<Dialogue>(m_dialogue->getId());
		auto script = "function _f(dialogue){\n" + m_conditionScript + "\nreturn false;}";
		return ActiveGame->getScriptManager()->call<bool>(script, "_f", dialogue);
	} catch (std::exception &e) {
		std::cerr << "DialogueSegment::conditionPasses() " << e.what() << std::endl;
		return false;
	}
}

std::string DialogueSegment::getText(bool *ok) const
{
	try {
		if (ok)
			*ok = true;
		if (m_scriptedText)
		{
			auto dialogue = GSave->get<Dialogue>(m_dialogue->getId());
			auto script = "function _f(dialogue){\n" + m_textRaw + "\nreturn \"\";}";
			return ActiveGame->getScriptManager()->call<std::string>(script, "_f", dialogue);
		} else
			return m_textRaw;
	} catch (std::exception &e) {
		if (ok)
			*ok = false;
		return e.what();
	}
}

std::pair<std::string,std::string> getLinePair(const std::string &line, const std::string &defaultName)
{
	auto result = std::make_pair(defaultName, line);
	if (line[0] != '[')
		return result;

	auto p = line.find(']');
	if (p == line.npos)
		return result;

	result.first = line.substr(1, p - 1);
	result.second = line.substr(p + 1);
	if (!result.second.empty() && result.second[0] == ' ')
		result.second = result.second.substr(1);
	return result;
}

std::vector<std::pair<std::string,std::string>> DialogueSegment::getTextMultiline(bool *ok) const
{
	std::vector<std::pair<std::string,std::string>> result;
	auto text = getText(ok);
	auto defaultName = m_dialogue ? m_dialogue->getDefaultName() : "";
	if (ok && !*ok) {
		result.push_back(getLinePair(text, defaultName));
		return result;
	}

	auto lines = split(text);
	std::string buffer;
	for (auto &line : lines)
	{
		if (line.empty()) {
			result.push_back(getLinePair(buffer, defaultName));
			buffer.clear();
		}
		else if (buffer.empty())
			buffer += line;
		else
			buffer += "\n" + line;
	}
	if (!buffer.empty())
		result.push_back(getLinePair(buffer, defaultName));

	return result;
}

bool DialogueSegment::isTextNext() const
{
	if (m_childrenIds.empty())
		return false;
	auto seg = m_dialogue->getSegment(m_childrenIds[0]);
	if (!seg->getTextRaw().empty())
		return seg->getType() == Text;
	return seg->isTextNext();
}

bool DialogueSegment::isOptionNext() const
{
	// TODO: Handle infinite recursion case with Link type
	if (m_childrenIds.empty())
		return false;
	auto seg = m_dialogue->getSegment(m_childrenIds[0]);
	if (!seg->getTextRaw().empty())
		return seg->getType() == Option;
	return seg->isOptionNext();
}

void DialogueSegment::setDialogue(Dialogue *dialogue)
{
	m_dialogue = dialogue;
}

void DialogueSegment::setTypeInt(int typeInt)
{
	m_type = static_cast<Type>(typeInt);
}

int DialogueSegment::getTypeInt() const
{
	return static_cast<int>(m_type);
}

json DialogueSegment::toJson() const
{
	auto jchildrenIds = sj::Array();
	for (auto childId : m_childrenIds)
		jchildrenIds.append(childId);

	json j = sj::Array(
		static_cast<int>(m_type),
		m_linkId,
		m_conditionalEnabled,
		m_scriptedText,
		m_scriptEnabled,
		m_showOnce,
		m_conditionScript,
		m_script,
		m_textRaw,
		jchildrenIds
	);
	return j;
}

bool DialogueSegment::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != 10)
		return false;

	m_type = static_cast<Type>(j[0].ToInt());
	m_linkId = j[1].ToInt();
	m_conditionalEnabled = j[2].ToBool();
	m_scriptedText = j[3].ToBool();
	m_scriptEnabled = j[4].ToBool();
	m_showOnce = j[5].ToBool();
	m_conditionScript = j[6].ToString();
	m_script = j[7].ToString();
	m_textRaw = j[8].ToString();
	m_childrenIds.clear();
	for (auto &jchildId : j[9].ArrayRange())
		m_childrenIds.push_back(jchildId.ToInt());

	return true;
}

} // namespace NovelTea
