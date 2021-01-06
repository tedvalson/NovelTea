#include <NovelTea/Game.hpp>
#include <NovelTea/GUI/TextOverlay.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>

#include <iostream>

namespace NovelTea
{

TextOverlay::TextOverlay()
: m_needsUpdate(true)
, m_alpha(255.f)
, m_nextStringIndex(0)
{
	m_bg.setFillColor(sf::Color(240.f, 240.f, 240.f));
	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);
	setSize(sf::Vector2f(150.f, 150.f));
}

// Returns true when it's time to close the overlay
bool TextOverlay::processEvent(const sf::Event &event)
{
	if (m_isShowing)
		return false;
	if (m_scrollBar.processEvent(event))
		return false;
	if (event.type == sf::Event::MouseButtonReleased)
	{
//		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		if (m_scrollPos > m_size.y - m_scrollAreaSize.y) {
			TweenEngine::Tween::to(m_scrollBar, ScrollBar::SCROLLPOS, 0.3f)
				.targetRelative(-m_size.y * 0.8f)
				.start(m_tweenManager);
			return false;
		} else
			return gotoNextString();
	}
	return false;
}

void TextOverlay::update(float delta)
{
	m_scrollBar.update(delta);
	Hideable::update(delta);
}

void TextOverlay::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionText();
}

float TextOverlay::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &TextOverlay::getScrollSize()
{
	return m_scrollAreaSize;
}

void TextOverlay::repositionText()
{
	m_text.setPosition(m_padding, m_padding + m_scrollPos);
	if (m_size.x > m_size.y)
		m_text.move(0.2f * m_size.x, 0.f);
}

void TextOverlay::show(float duration, int tweenType, HideableCallback callback)
{
	Hideable::show(duration, tweenType, [this, callback](){
		gotoNextString(callback);
	});
}

void TextOverlay::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, [this, callback](){
		// Reset scrollbar positioning
		m_scrollAreaSize.y = 0.f;
		updateScrollbar();
		if (callback)
			callback();
	});
}

void TextOverlay::setText(const std::string &text)
{
	std::vector<std::string> strings;
	strings.push_back(text);
	setTextArray(strings);
}

void TextOverlay::setTextArray(const std::vector<std::string> &textArray)
{
	m_strings = textArray;
	m_nextStringIndex = 0;
	m_text.setText("");
	if (isVisible() && !isShowing())
		gotoNextString();
}

void TextOverlay::setAlpha(float alpha)
{
	sf::Color color;
	m_alpha = alpha;
	float *newValues = &alpha;
	m_text.setAlpha(alpha);
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
}

float TextOverlay::getAlpha() const
{
	return m_alpha;
}

void TextOverlay::setSize(const sf::Vector2f &size)
{
	auto portrait = size.x < size.y;
	m_padding = 1.f / 8.f * (portrait ? size.x : size.y);
	m_needsUpdate = true;

	m_view.reset(sf::FloatRect(0.f, 0.f, size.x, size.y));

	m_bg.setSize(size);
	m_text.setSize(sf::Vector2f((portrait ? 1.f : 0.6f) * size.x - m_padding * 2.f, size.y));

	m_scrollBar.setPosition(size.x - 4.f, 4.f);
	m_scrollBar.setSize(sf::Vector2u(2, size.y - 8.f));
	m_scrollBar.setScrollAreaSize(sf::Vector2u(size.x, size.y));

	m_size = size;

	m_scrollAreaSize.y = m_padding*2 + m_text.getLocalBounds().height;
	m_scrollBar.setScroll(0.f);
}

sf::Vector2f TextOverlay::getSize() const
{
	return m_size;
}

sf::FloatRect TextOverlay::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect TextOverlay::getGlobalBounds() const
{
	ensureUpdate();
	return getTransform().transformRect(m_bounds);
}

void TextOverlay::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	m_needsUpdate = false;
}

bool TextOverlay::gotoNextString(HideableCallback callback)
{
	if (m_nextStringIndex >= m_strings.size())
		return true;

	m_oldText = m_text;
	m_text.setText(m_strings[m_nextStringIndex]);
	m_text.setAlpha(0.f);

	m_scrollBar.setScroll(0.f);
	m_scrollAreaSize.y = m_padding*2 + m_text.getLocalBounds().height;
	updateScrollbar();

	TweenEngine::Tween::to(m_oldText, ActiveText::ALPHA, 0.5f)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_text, ActiveText::ALPHA, 0.5f)
		.target(255.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [callback](TweenEngine::BaseTween*){
			if (callback)
				callback();
		}).start(m_tweenManager);

	m_nextStringIndex++;
	return false;
}

void TextOverlay::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!isVisible())
		return;
	auto view = target.getView();
	ensureUpdate();
	states.transform *= getTransform();
	m_view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, m_size.y / target.getSize().y));

	target.draw(m_bg, states);

	target.setView(m_view);
	target.draw(m_oldText, states);
	target.draw(m_text, states);
	target.setView(view);

	target.draw(m_scrollBar, states);
}

} // namespace NovelTea
