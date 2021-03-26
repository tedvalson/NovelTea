#ifndef NOVELTEA_CUTSCENERENDERER_HPP
#define NOVELTEA_CUTSCENERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/GUI/Icon/IconGlow.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <NovelTea/json.hpp>
#include <memory>

namespace NovelTea
{

class Cutscene;
class CutsceneSegment;
class CutsceneTextSegment;
class CutscenePageBreakSegment;
class CutscenePageSegment;
class ActiveText;

class CutsceneRenderer : public sf::Drawable, public Scrollable, public TweenTransformable<sf::Transformable>
{
public:
	CutsceneRenderer();

	void setCutscene(const std::shared_ptr<Cutscene> &cutscene);
	const std::shared_ptr<Cutscene> &getCutscene() const;
	void reset(bool preservePosition = false);

	bool processEvent(const sf::Event &event);
	void update(float delta);

	bool isComplete() const;
	bool isWaitingForClick() const;
	void click();

	sj::JSON saveState() const;
	void restoreState(const sj::JSON &jstate);

	void setScrollTween(float position, float duration);
	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;
	void repositionItems();

	void setSize(const sf::Vector2f &size);
	const sf::Vector2f &getSize() const;

	void setMargin(float margin);
	float getMargin() const;

	void setFontSizeMultiplier(float fontSizeMultiplier);
	float getFontSizeMultiplier() const;

	ADD_ACCESSOR(bool, SkipWaitingForClick, m_skipWaitingForClick)
	ADD_ACCESSOR(bool, SkipScriptSegments, m_skipScriptSegments)

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void startTransitionEffect(const CutsceneTextSegment *segment);
	void startTransitionEffect(const CutscenePageBreakSegment *segment);
	void addSegmentToQueue(size_t segmentIndex);

private:
	std::shared_ptr<Cutscene> m_cutscene;
	std::shared_ptr<CutsceneSegment> m_currentSegment;
	int m_segmentIndex;
	int m_segmentSaveIndex;
	bool m_isComplete;
	bool m_isWaitingForClick;
	bool m_skipWaitingForClick;
	bool m_skipScriptSegments;

	IconGlow m_icon;

	std::vector<std::shared_ptr<ActiveText>> m_texts;
	std::vector<std::shared_ptr<ActiveText>> m_textsOld;
	sf::Vector2f m_cursorPos;
	sf::Vector2f m_size;
	float m_margin;
	float m_fontSizeMultiplier;

	float m_scrollPos;
	ScrollBar m_scrollBar;
	sf::Vector2f m_scrollAreaSize;
	sf::Transform m_scrollTransform;
	sf::Transform m_scrollTransformOld;

	sf::RectangleShape m_fadeRectTop;
	sf::RectangleShape m_fadeRectBottom;

	sf::Time m_timePassed;
	sf::Time m_timeToNext;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_CUTSCENERENDERER_HPP
