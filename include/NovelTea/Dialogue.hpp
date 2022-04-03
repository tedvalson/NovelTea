#ifndef NOVELTEA_DIALOGUE_HPP
#define NOVELTEA_DIALOGUE_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class DialogueSegment;

enum class DialogueTextLogMode {
	Nothing = 0,
	Everything,
	OnlyText,
	OnlyOptions,
};

class Dialogue : public Entity
{
public:
	Dialogue();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	void clearSegments();
	void addSegment(std::shared_ptr<DialogueSegment> segment);
	std::shared_ptr<DialogueSegment> getSegment(int index, bool followLink = true) const;
	std::vector<std::shared_ptr<DialogueSegment>> &segments();

	void setSegmentHasShown(int index, int subIndex = -1);
	bool getSegmentHasShown(int index, int subIndex = -1) const;
	bool segmentShown(int index, int subIndex = -1);

	static constexpr auto id = ID::Dialogue;
	const std::string entityId() const override {return id;}

	void setNextEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> getNextEntity() const;

	ADD_ACCESSOR(int, RootIndex, m_rootIndex)
	ADD_ACCESSOR(bool, EnableDisabledOptions, m_enableDisabledOptions)
	ADD_ACCESSOR(bool, ShowDisabledOptions, m_showDisabledOptions)
	ADD_ACCESSOR(std::string, DefaultName, m_defaultName)
	ADD_ACCESSOR(DialogueTextLogMode, LogMode, m_logMode)
	ADD_ACCESSOR(json, NextEntityJson, m_nextEntity)

private:
	int m_rootIndex;
	bool m_enableDisabledOptions;
	bool m_showDisabledOptions;
	std::vector<std::shared_ptr<DialogueSegment>> m_segments;
	std::string m_defaultName;
	DialogueTextLogMode m_logMode;
	json m_nextEntity;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUE_HPP
