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
	std::vector<std::shared_ptr<DialogueSegment>> &segments();

	static constexpr auto id = "dialogue";
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(int, RootIndex, m_rootIndex)
	ADD_ACCESSOR(json, NextEntity, m_nextEntity)

private:
	int m_rootIndex;
	std::vector<std::shared_ptr<DialogueSegment>> m_segments;
	json m_nextEntity;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUE_HPP
