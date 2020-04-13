#ifndef NOVELTEA_CUTSCENE_HPP
#define NOVELTEA_CUTSCENE_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <vector>

namespace NovelTea
{

class CutsceneSegment;

class Cutscene: public JsonSerializable
{
public:
	Cutscene();
	~Cutscene();
	virtual json toJson() const;
	virtual bool fromJson(const json &j);

	const std::vector<std::shared_ptr<CutsceneSegment>> &segments() const;

	Cutscene(Cutscene&&) = default;
	Cutscene& operator=(Cutscene&&) = default;
	Cutscene(const Cutscene&) = delete;
	Cutscene& operator=(const Cutscene&) = delete;

private:
	std::string m_name;
	std::vector<std::shared_ptr<CutsceneSegment>> m_segments;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENE_HPP
