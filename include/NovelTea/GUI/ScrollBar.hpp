#ifndef NOVELTEA_SCROLLBAR_HPP
#define NOVELTEA_SCROLLBAR_HPP

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <TweenEngine/TweenManager.h>
#include "Scrollable.hpp"
#include "NinePatch.hpp"
#include "../TweenObjects.hpp"

namespace NovelTea
{

class ScrollBar : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	static const int ALPHA = 11; // for tweening
	static const int SCROLLPOS = 12;

	ScrollBar();
	~ScrollBar();
	void attachObject(Scrollable *object);
	void detachObject(Scrollable *object);

	void setScroll(float scrollPos);
	void setScrollRelative(float scrollDelta);

	void setPosition(const sf::Vector2f& position);
	void setPosition(float x, float y);
	const sf::Vector2f& getPosition() const;

	void setDragRect(const sf::FloatRect &rect);
	void setSize(const sf::Vector2f &size);
	void setScrollAreaSize(const sf::Vector2f &size);
	void setDeceleration(float rate);
	void setColor(const sf::Color &color);
	void setAutoHide(bool autoHide);

	sf::FloatRect getDragRect() const;
	sf::Vector2f getSize() const;
	sf::Vector2f getScrollAreaSize() const;
	float getDeceleration() const;
	sf::Color getColor() const;
	bool getAutoHide() const;

	void show();
	void hide();
	void markDirty();

	// Returns true when event is captured
	bool processEvent(const sf::Event &event);
	void update(float delta);

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void setValues(int tweenType, float *newValues);
	virtual int getValues(int tweenType, float *returnValues);
	void ensureUpdateScrollBar() const;

private:
	bool m_autoHide;
	bool m_isHidden;
	bool m_isTouching;
	bool m_isScrolling;
	mutable float m_scrollPos;
	mutable float m_scrollPosMin;
	mutable float m_scrollPosMax;
	mutable float m_scrollSize;
	float m_scrollTolerance; // Value to move before scrolling starts
	sf::Vector2i m_lastTouchPos;

	sf::Clock m_clockHide;
	sf::Color m_color;

	mutable bool m_needsUpdate;
	mutable std::vector<Scrollable*> m_scrollObjects;
	sf::FloatRect m_dragRect;
	sf::Vector2f m_scrollAreaSize;
	sf::Vector2f m_size;
	sf::Vector2f m_position;

	NinePatch m_scrollBar;
	TweenEngine::TweenManager m_tweenManager;

	// Kinetic scrolling
	float m_deceleration;   // Rate at which the velocity slows to a stop (1.f to never end, 0.f for instant stop)
	float m_velocity;       // Store current velocity
	float m_veloctyModifer; // For fine tuning velocity
	sf::Clock m_clockVelocity;
	sf::Vector2i m_startTouchPos;
};

} // namespace NovelTea


#endif // NOVELTEA_SCROLLBAR_HPP
