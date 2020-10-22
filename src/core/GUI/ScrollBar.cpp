#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/AssetManager.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

ScrollBar::ScrollBar()
: m_autoHide(true)
, m_isHidden(true)
, m_isTouching(false)
, m_isScrolling(false)
, m_scrollPos(0.f)
, m_scrollPosMin(0.f)
, m_scrollPosMax(0.f)
, m_scrollTolerance(6.f)
, m_needsUpdate(true)
, m_deceleration(0.85f)
, m_velocity(0.f)
, m_veloctyModifer(2000.f)
{
	auto texture = AssetManager<sf::Texture>::get("images/scrollbar.9.png");
	texture->setSmooth(true);
	m_scrollBar.setTexture(texture.get());
	m_scrollBar.setContentSize(0.f, 100.f);
	m_scrollBar.setColor(sf::Color::Red);
}

ScrollBar::~ScrollBar()
{
	for (auto &obj : m_scrollObjects)
		obj->detachScrollbar();
}

void ScrollBar::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdateScrollBar();
	if (m_scrollBar.getSize().y < m_size.y)
	{
		states.transform *= getTransform();
		target.draw(m_scrollBar, states);
	}
}

void ScrollBar::attachObject(Scrollable *object)
{
	if (!object)
		return;
	if (std::find(m_scrollObjects.begin(), m_scrollObjects.end(), object) == m_scrollObjects.end())
	{
		object->attachScrollbar(this);
		m_scrollObjects.push_back(object);
		m_needsUpdate = true;
	}
}

void ScrollBar::detachObject(Scrollable *object)
{
	if (!object)
		return;
	for (auto it = m_scrollObjects.begin(); it != m_scrollObjects.end(); ++it)
		if (*it == object)
		{
			m_scrollObjects.erase(it);
			object->detachScrollbar();
			m_needsUpdate = true;
			break;
		}
}

bool ScrollBar::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseMoved)
	{
		if (m_isTouching)
		{
			m_clockHide.restart();
			if (!m_isScrolling && std::abs(m_startTouchPos.y - event.mouseButton.y) > m_scrollTolerance)
				m_isScrolling = true;

			float posDiff = m_lastTouchPos.y - event.mouseButton.y;
			if (m_isScrolling)
				setScrollRelative(-posDiff);
			m_lastTouchPos.x = event.mouseButton.x;
			m_lastTouchPos.y = event.mouseButton.y;

			sf::Time timeDiff = m_clockVelocity.restart();
			float v = m_veloctyModifer * -posDiff / (1 + timeDiff.asMilliseconds());
			if ((v >= 0.f && m_velocity >= 0.f) || (v < 0.f && m_velocity < 0.f))
				m_velocity = 0.5f * v + 0.5f * m_velocity;
			else
				m_velocity = v;

			return m_isScrolling;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (!m_isTouching && (m_dragRect == sf::FloatRect() || m_dragRect.contains(event.mouseButton.x, event.mouseButton.y)))
		{
			m_isTouching = true;
			m_isScrolling = false;
			m_startTouchPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
			m_lastTouchPos = m_startTouchPos;
			m_clockVelocity.restart();
			m_velocity = 0.f;
			return m_isScrolling;
		}
	}
	else if (m_isTouching && event.type == sf::Event::MouseButtonReleased)
	{
		m_isTouching = false;
		return m_isScrolling || m_lastTouchPos != m_startTouchPos;
	}
	return false;
}

void ScrollBar::update(float delta)
{
	if (m_autoHide && m_clockHide.getElapsedTime() > sf::seconds(3.f))
		hide();

	if (!m_autoHide && m_isHidden)
		show();

	if (m_isScrolling && !m_isTouching)
	{
		if (m_velocity > 0.5f || m_velocity < -0.5f)
		{
			m_velocity *= m_deceleration;
			setScrollRelative(m_velocity * delta);
		}
		else
			m_isScrolling = false;
	}

	if (m_isScrolling)
	{
		for (auto &obj : m_scrollObjects)
			obj->setScroll(m_scrollPos);
		// If scrolling, and finger stops for a certain time, stop scroll
		if (m_clockVelocity.getElapsedTime().asSeconds() > 0.1f)
			m_isScrolling = false;
	}

	if (m_scrollPos > m_scrollPosMax)
		setScroll(m_scrollPosMax);

	m_tweenManager.update(delta);
}

void ScrollBar::setDragRect(const sf::FloatRect &rect)
{
	m_dragRect = rect;
	m_needsUpdate = true;
}

void ScrollBar::setScrollAreaSize(const sf::Vector2u &size)
{
	m_scrollAreaSize = size;
	m_needsUpdate = true;
}

void ScrollBar::setDeceleration(float rate)
{
	m_deceleration = rate;
}

void ScrollBar::setSize(const sf::Vector2u &size)
{
	m_size = size;
	m_needsUpdate = true;
}

void ScrollBar::ensureUpdateScrollBar() const
{
	if (!m_needsUpdate)
		return;

	m_scrollSize = 0.f;
	m_scrollPosMin = m_scrollAreaSize.y;
	m_scrollPosMax = 0.f;
	for (auto &obj : m_scrollObjects)
	{
		const sf::Vector2f &size = obj->getScrollSize();
		m_scrollSize += size.y;
		m_scrollPos = obj->getScroll();
	}
	m_scrollPosMin = m_scrollAreaSize.y - m_scrollSize;

	// If everything is showing, no scrolling to do
	if (m_scrollPosMin > 0)
		m_scrollPosMin = 0;

	float visibleRatio = m_scrollAreaSize.y / (m_scrollSize+1);
	if (visibleRatio < 0.05f)
		visibleRatio = 0.05f;
	m_scrollBar.setSize(0.f, m_size.y * visibleRatio);

	m_needsUpdate = false;
}

void ScrollBar::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case ALPHA: {
			sf::Color color = m_scrollBar.getColor();
			color.a = newValues[0];
			m_scrollBar.setColor(color);
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int ScrollBar::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case ALPHA:
			returnValues[0] = m_scrollBar.getColor().a;
			return 1;
		default:
			return TweenTransformable::getValues(tweenType, returnValues);
	}
}

void ScrollBar::show()
{
	if (!m_isHidden || m_tweenManager.getRunningTweensCount() > 0)
		return;
	m_clockHide.restart();
	TweenEngine::Tween::to(*this, ALPHA, 0.5f)
		.target(m_color.a)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*) {
			m_isHidden = false;
		})
		.start(m_tweenManager);
}

void ScrollBar::hide()
{
	if (m_isHidden || m_tweenManager.getRunningTweensCount() > 0)
		return;
	TweenEngine::Tween::to(*this, ALPHA, 0.8f)
		.target(0)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*) {
			m_isHidden = true;
		})
		.start(m_tweenManager);
}

void ScrollBar::setColor(const sf::Color &color)
{
	sf::Color tmpColor = color;
	if (m_isHidden)
		tmpColor.a = 0;

	m_color = color;
	m_scrollBar.setColor(tmpColor);
}

void ScrollBar::setAutoHide(bool autoHide)
{
	m_autoHide = autoHide;
}

sf::FloatRect ScrollBar::getDragRect() const
{
	return m_dragRect;
}

sf::Vector2u ScrollBar::getSize() const
{
	return m_size;
}

sf::Vector2u ScrollBar::getScrollAreaSize() const
{
	return m_scrollAreaSize;
}

float ScrollBar::getDeceleration() const
{
	return m_deceleration;
}

sf::Color ScrollBar::getColor() const
{
	return m_scrollBar.getColor();
}

bool ScrollBar::getAutoHide() const
{
	return m_autoHide;
}

void ScrollBar::setScroll(float scrollPos)
{
	ensureUpdateScrollBar();
	// Reset velocity to end scrolling when limits met
	if (scrollPos > m_scrollPosMax) {
		scrollPos = m_scrollPosMax;
		m_velocity = 0.f;
	} else if (scrollPos < m_scrollPosMin) {
		scrollPos = m_scrollPosMin;
		m_velocity = 0.f;
	}

	m_scrollPos = scrollPos;
	if (m_autoHide && m_scrollPosMin != m_scrollPosMax)
		show();

	setPosition(m_position);
}

void ScrollBar::setScrollRelative(float scrollDelta)
{
	setScroll(m_scrollPos + scrollDelta);
}

void ScrollBar::setPosition(const sf::Vector2f &position)
{
	m_position = position;
	float visibleRatio = m_size.y / (m_scrollSize+1);
	Transformable::setPosition(position + sf::Vector2f(0.f, -m_scrollPos * visibleRatio));
}

void ScrollBar::setPosition(float x, float y)
{
	sf::Vector2f position(x, y);
	setPosition(position);
}

const sf::Vector2f &ScrollBar::getPosition() const
{
	return m_position;
}

void ScrollBar::markDirty()
{
	m_needsUpdate = true;
}


} // namespace NovelTea
