#ifndef NOVELTEA_CUTSCENE_PLAYER_HPP
#define NOVELTEA_CUTSCENE_PLAYER_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/json.hpp>
#include <TweenEngine/TweenManager.h>
#include <memory>

namespace NovelTea
{

class Cutscene;
class CutsceneSegment;

class CutscenePlayer : public ContextObject
{
public:
	CutscenePlayer(Context* context);
	virtual ~CutscenePlayer();

	void setCutscene(const std::shared_ptr<Cutscene> &cutscene);
	const std::shared_ptr<Cutscene> &getCutscene() const;
	void reset(bool preservePosition = false);

	void update(float delta);

	bool isComplete() const;
	bool isWaitingForClick() const;
	void click();

	sj::JSON saveState() const;
	void restoreState(const sj::JSON &jstate);

	int getSegmentIndex() const;
	int getInternalSegmentIndex(int index = -1) const;

	ADD_ACCESSOR(bool, SkipWaitingForClick, m_skipWaitingForClick)
	ADD_ACCESSOR(bool, SkipScriptSegments, m_skipScriptSegments)
	ADD_ACCESSOR(bool, SkipConditionChecks, m_skipConditionChecks)

protected:
	void addSegmentToQueue(size_t segmentIndex);
	void waitForClick();

private:
	std::shared_ptr<Cutscene> m_cutscene;
	std::shared_ptr<CutsceneSegment> m_currentSegment;
	int m_segmentIndex;
	int m_segmentSaveIndex;
	bool m_isComplete;
	bool m_isWaitingForClick;
	bool m_skipWaitingForClick;
	bool m_skipScriptSegments;
	bool m_skipConditionChecks;

	float m_timePassed;
	float m_timeToNext;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENE_PLAYER_HPP
