#ifndef NOVELTEA_DIALOGUE_HPP
#define NOVELTEA_DIALOGUE_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class DialogueSegment;

class Dialogue : public Entity
{
public:
	Dialogue();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	void clearSegments();
	void addSegment(std::shared_ptr<DialogueSegment> segment);
	std::shared_ptr<DialogueSegment> getSegment(int index, bool followLink = true) const;
	std::vector<std::shared_ptr<DialogueSegment>> &segments();

	void setSegmentHasShown(int index);
	bool getSegmentHasShown(int index) const;
	bool segmentShown(int index);

	static constexpr auto id = "dialogue";
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(int, RootIndex, m_rootIndex)
	ADD_ACCESSOR(std::string, DefaultName, m_defaultName)
	ADD_ACCESSOR(json, NextEntity, m_nextEntity)

private:
	int m_rootIndex;
	std::vector<std::shared_ptr<DialogueSegment>> m_segments;
	std::string m_defaultName;
	json m_nextEntity;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUE_HPP
