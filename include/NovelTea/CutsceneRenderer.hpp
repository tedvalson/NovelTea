#ifndef NOVELTEA_CUTSCENERENDERER_HPP
#define NOVELTEA_CUTSCENERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <memory>

namespace NovelTea
{

class Cutscene;
class CutsceneTextSegment;
class CutscenePageBreakSegment;
class ActiveText;

class CutsceneRenderer : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	CutsceneRenderer();

	void setCutscene(const std::shared_ptr<Cutscene> &cutscene);
	void reset();
	void update(float delta);

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void startTransitionEffect(const CutsceneTextSegment *segment);
	void startTransitionEffect(const CutscenePageBreakSegment *segment);
	void addSegmentToQueue(size_t segmentIndex);

private:
	std::shared_ptr<Cutscene> m_cutscene;
	size_t m_segmentIndex;

	std::vector<std::shared_ptr<ActiveText>> m_texts;
	std::vector<std::shared_ptr<ActiveText>> m_textsOld;
	sf::Vector2f m_cursorPos;

	sf::Time m_timePassed;
	sf::Time m_timeToNext;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENERENDERER_HPP
