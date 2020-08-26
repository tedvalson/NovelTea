#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Game.hpp>

namespace NovelTea
{

DialogueSegment::DialogueSegment()
: m_id(-1)
, m_linkId(-1)
, m_type(Type::Invalid)
, m_conditionalEnabled(false)
, m_scriptedText(false)
, m_scriptEnabled(false)
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
		ActiveGame->getScriptManager().runInClosure(m_script);
	} catch (std::exception &e) {
		std::cerr << "DialogueSegment::runScript() " << e.what() << std::endl;
	}
}

bool DialogueSegment::conditionPasses() const
{
	if (!m_conditionalEnabled || m_conditionScript.empty())
		return true;
	try {
		auto script = m_conditionScript;
		if (m_dialogue)
			script = "dialogue=Save.loadDialogue('"+m_dialogue->getId()+"');\n" + m_conditionScript;
		auto result = ActiveGame->getScriptManager().runInClosure<bool>(script);
		return result;
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
			auto script = m_textRaw;
			if (m_dialogue)
				script = "dialogue=Save.loadDialogue('"+m_dialogue->getId()+"');\n" + m_textRaw;
			return ActiveGame->getScriptManager().runInClosure<std::string>(script);
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

void DialogueSegment::setDialogue(Dialogue *dialogue)
{
	m_dialogue = dialogue;
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
		m_conditionScript,
		m_script,
		m_textRaw,
		jchildrenIds
	);
	return j;
}

bool DialogueSegment::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != 9)
		return false;

	m_type = static_cast<Type>(j[0].ToInt());
	m_linkId = j[1].ToInt();
	m_conditionalEnabled = j[2].ToBool();
	m_scriptedText = j[3].ToBool();
	m_scriptEnabled = j[4].ToBool();
	m_conditionScript = j[5].ToString();
	m_script = j[6].ToString();
	m_textRaw = j[7].ToString();
	m_childrenIds.clear();
	for (auto &jchildId : j[8].ArrayRange())
		m_childrenIds.push_back(jchildId.ToInt());

	return true;
}

} // namespace NovelTea
