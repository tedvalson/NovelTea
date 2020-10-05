#ifndef NOVELTEA_CUTSCENERENDERER_HPP
#define NOVELTEA_CUTSCENERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/Scrollable.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <NovelTea/json.hpp>
#include <memory>

namespace NovelTea
{

class Cutscene;
class CutsceneTextSegment;
class CutscenePageBreakSegment;
class ActiveText;

class CutsceneRenderer : public sf::Drawable, public Scrollable, public TweenTransformable<sf::Transformable>
{
public:
	static const int _SCROLLPOS = 11;

	CutsceneRenderer();

	void setCutscene(const std::shared_ptr<Cutscene> &cutscene);
	void reset();
	void update(float delta);

	bool isComplete() const;
	bool isWaitingForClick() const;
	void click();

	void setScrollTween(float position, float duration);
	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;
	void repositionItems();

	ADD_ACCESSOR(sf::Vector2f, Size, m_size)
	ADD_ACCESSOR(float, Margin, m_margin)
	ADD_ACCESSOR(bool, SkipWaitingForClick, m_skipWaitingForClick)

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void startTransitionEffect(const CutsceneTextSegment *segment);
	void startTransitionEffect(const CutscenePageBreakSegment *segment);
	void addSegmentToQueue(size_t segmentIndex);

	virtual void setValues(int tweenType, float *newValues) override;
	virtual int getValues(int tweenType, float *returnValues) override;

private:
	std::shared_ptr<Cutscene> m_cutscene;
	size_t m_segmentIndex;
	bool m_isComplete;
	bool m_isWaitingForClick;
	bool m_skipWaitingForClick;

	std::vector<std::shared_ptr<ActiveText>> m_texts;
	std::vector<std::shared_ptr<ActiveText>> m_textsOld;
	sf::Vector2f m_cursorPos;
	sf::Vector2f m_size;
	float m_margin;

	float m_scrollPos;
	sf::Vector2f m_scrollAreaSize;
	sf::Transform m_scrollTransform;
	sf::Transform m_scrollTransformOld;

	sf::Time m_timePassed;
	sf::Time m_timeToNext;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENERENDERER_HPP
