#ifndef NOVELTEA_CUTSCENESEGMENT_HPP
#define NOVELTEA_CUTSCENESEGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>

namespace NovelTea
{

class CutsceneSegment : public JsonSerializable
{
public:
	enum Type {
		Text      = 0,
		PageBreak = 1,
	};

	CutsceneSegment();
	virtual ~CutsceneSegment();

	virtual Type type() const = 0;

	static std::shared_ptr<CutsceneSegment> createSegment(const json &j);

	virtual void setDuration(size_t duration);
	virtual size_t getDuration() const;

	void setScriptOverride(bool override);
	bool getScriptOverride() const;

	void setScriptOverrideName(const std::string &scriptName);
	const std::string &getScriptOverrideName() const;

private:
	size_t m_duration = 1000;
	bool m_scriptOverride = false;
	std::string m_scriptOverrideName;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENESEGMENT_HPP
