#ifndef NOVELTEA_CUTSCENE_HPP
#define NOVELTEA_CUTSCENE_HPP

#include <NovelTea/Entity.hpp>
#include <vector>

namespace NovelTea
{

class CutsceneSegment;

class Cutscene : public Entity
{
public:
	Cutscene(Context *context);
	~Cutscene();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Cutscene;
	const std::string entityId() const override {return id;}

	void addSegment(std::shared_ptr<CutsceneSegment> segment);
	void updateSegments();

	// TODO: make CutsceneWidget a friend to not expose this entirely
	std::vector<std::shared_ptr<CutsceneSegment>> &segments();
	std::vector<std::shared_ptr<CutsceneSegment>> &internalSegments();

	size_t getDurationMs() const;
	size_t getDurationMs(size_t indexEnd) const;

	size_t getDelayMs() const;
	size_t getDelayMs(size_t indexEnd) const;

	Cutscene(Cutscene&&) = default;
	Cutscene& operator=(Cutscene&&) = default;
	Cutscene(const Cutscene&) = delete;
	Cutscene& operator=(const Cutscene&) = delete;

	void setNextEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> getNextEntity() const;

	ADD_ACCESSOR(bool, FullScreen, m_fullScreen)
	ADD_ACCESSOR(bool, CanFastForward, m_canFastForward)
	ADD_ACCESSOR(float, SpeedFactor, m_speedFactor)
	ADD_ACCESSOR(json, NextEntityJson, m_nextEntity)
	ADD_ACCESSOR(bool, SkipConditionChecks, m_skipConditionChecks)

private:
	std::vector<std::shared_ptr<CutsceneSegment>> m_segments;
	std::vector<std::shared_ptr<CutsceneSegment>> m_internalSegments;

	bool m_fullScreen;
	bool m_canFastForward;
	float m_speedFactor;
	json m_nextEntity;

	bool m_skipConditionChecks;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENE_HPP
