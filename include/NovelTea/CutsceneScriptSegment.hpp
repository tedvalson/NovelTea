#ifndef NOVELTEA_CUTSCENESCRIPTSEGMENT_HPP
#define NOVELTEA_CUTSCENESCRIPTSEGMENT_HPP

#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Cutscene;

class CutsceneScriptSegment : public CutsceneSegment
{
public:
	CutsceneScriptSegment();
	json toJson() const override;
	bool fromJson(const json &j) override;
	Type type() const override;

	void runScript(const std::shared_ptr<Cutscene> &cutscene) const;

	ADD_ACCESSOR(bool, AutosaveAfter, m_autosaveAfter)
	ADD_ACCESSOR(bool, AutosaveBefore, m_autosaveBefore)
	ADD_ACCESSOR(std::string, Comment, m_comment)
	ADD_ACCESSOR(std::string, Script, m_script)

private:
	bool m_autosaveAfter;
	bool m_autosaveBefore;
	std::string m_comment;
	std::string m_script;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENESCRIPTSEGMENT_HPP
