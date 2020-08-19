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
	setSize(sf::Vector2f(150.f, 150.f));
}

bool TextOverlay::processEvent(const sf::Event &event)
{
	if (m_isShowing)
		return false;
	if (event.type == sf::Event::MouseButtonPressed)
	{
//		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		return gotoNextString();
	}
	return false;
}

void TextOverlay::show(float duration, int tweenType, HideableCallback callback)
{
	Hideable::show(duration, tweenType, [this, callback](){
		gotoNextString(callback);
	});
}

void TextOverlay::setText(const std::string &text)
{
	m_strings.clear();
	m_strings.push_back(text);
	m_nextStringIndex = 0;
	m_text.setText("");
}

void TextOverlay::setTextArray(const std::vector<std::string> &textArray)
{
	m_strings = textArray;
	m_nextStringIndex = 0;
	m_text.setText("");
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
	auto padding = 1.f / 8.f * size.x;
	m_needsUpdate = true;
	m_bg.setSize(size);
	m_text.setSize(sf::Vector2f(size.x - padding * 2.f, size.y));
	m_text.setPosition(padding, padding);
	m_size = size;
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
	ensureUpdate();
	states.transform *= getTransform();
	target.draw(m_bg, states);
	target.draw(m_oldText, states);
	target.draw(m_text, states);
}

} // namespace NovelTea
