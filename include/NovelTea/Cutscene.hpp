#ifndef NOVELTEA_CUTSCENE_HPP
#define NOVELTEA_CUTSCENE_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <vector>

namespace NovelTea
{

class CutsceneSegment;

class Cutscene : public JsonSerializable
{
public:
	Cutscene();
	~Cutscene();
	json toJson() const override;
	bool fromJson(const json &j) override;

	void addSegment(std::shared_ptr<CutsceneSegment> segment);

	// TODO: make CutsceneWidget a friend to not expose this entirely
	std::vector<std::shared_ptr<CutsceneSegment>> &segments();

	size_t getDurationMs() const;
	size_t getDurationMs(size_t indexEnd) const;

	size_t getDelayMs() const;
	size_t getDelayMs(size_t indexEnd) const;

	Cutscene(Cutscene&&) = default;
	Cutscene& operator=(Cutscene&&) = default;
	Cutscene(const Cutscene&) = delete;
	Cutscene& operator=(const Cutscene&) = delete;

	ADD_ACCESSOR(bool, FullScreen, m_fullScreen)
	ADD_ACCESSOR(bool, CanFastForward, m_canFastForward)
	ADD_ACCESSOR(float, SpeedFactor, m_speedFactor)

private:
	std::string m_name;
	std::vector<std::shared_ptr<CutsceneSegment>> m_segments;

	bool m_fullScreen;
	bool m_canFastForward;
	float m_speedFactor;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENE_HPP
