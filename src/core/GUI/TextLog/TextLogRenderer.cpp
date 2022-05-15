#include <NovelTea/GUI/TextLog/TextLogRenderer.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/GUI/TextLog/TextLogGenericItem.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/System/Err.hpp>

namespace NovelTea
{

TextLogRenderer::TextLogRenderer()
: m_needsUpdate(true)
, m_mousePressed(false)
, m_alpha(255.f)
{
	m_bg.setFillColor(sf::Color(240.f, 240.f, 240.f));
	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	auto& entries = ActiveGame->getTextLog()->entries();
	sf::err() << "loading text log... " << entries.size() << std::endl;
	for (auto it = entries.begin(); it != entries.end(); ++it)
	{
		TextLogItem* item = nullptr;
		auto type = it->type;

		if (type == TextLogType::DialogueTextName) {
			auto name = it->text;
			if (++it != entries.end())
				item = new TextLogGenericItem(name + " -- " + it->text);
		} else {
			item = new TextLogGenericItem(it->text);
		}

		if (item)
			m_items.emplace_back(item);
	}
}

// Returns true when it's time to close the overlay
bool TextLogRenderer::processEvent(const sf::Event &event)
{
	if (m_isShowing)
		return false;
	if (m_scrollBar.processEvent(event) && event.type != sf::Event::MouseButtonPressed)
	{
		m_mousePressed = false;
		return false;
	}
	return false;
}

bool TextLogRenderer::update(float delta)
{
	m_scrollBar.update(delta);
	Hideable::update(delta);
	return true;
}

void TextLogRenderer::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionText();
}

float TextLogRenderer::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &TextLogRenderer::getScrollSize()
{
	return m_scrollAreaSize;
}

void TextLogRenderer::repositionText()
{
	m_view.setCenter({m_size.x / 2.f, m_size.y / 2.f - m_scrollPos});
}

void TextLogRenderer::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
}

void TextLogRenderer::show(float duration, int tweenType, HideableCallback callback)
{
	Hideable::show(duration, tweenType, callback);
}

void TextLogRenderer::hide(float duration, int tweenType, HideableCallback callback)
{
	Hideable::hide(duration, tweenType, [this, callback](){
		// Reset scrollbar positioning
		m_scrollAreaSize.y = 0.f;
		updateScrollbar();
		if (callback)
			callback();
	});
}

void TextLogRenderer::setAlpha(float alpha)
{
	sf::Color color;
	m_alpha = alpha;
	float *newValues = &alpha;
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
	for (auto& item : m_items)
	{
		item->setAlpha(alpha);
	}
}

float TextLogRenderer::getAlpha() const
{
	return m_alpha;
}

void TextLogRenderer::setSize(const sf::Vector2f &size)
{
	sf::err() << "TextLogRenderer::setSize" << std::endl;
	auto portrait = size.x < size.y;
	m_padding = 1.f / 8.f * (portrait ? size.x : size.y);
	m_needsUpdate = true;

	m_view.reset(sf::FloatRect(0.f, 0.f, size.x, size.y));

	m_bg.setSize(size);

	m_scrollBar.setPosition(size.x - 4.f, 4.f);
	m_scrollBar.setSize(sf::Vector2f(2, size.y - 8.f));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(size.x, size.y));

	m_size = size;

	auto posY = 0.f;
	for (auto& item : m_items)
	{
		item->setFontSizeMultiplier(m_fontSizeMultiplier);
		item->setWidth(size.x);
		item->setPosition({0.f, posY});
		posY += item->getLocalBounds().height;
	}

	m_scrollAreaSize.y = posY;
	m_scrollBar.setScroll(-posY);
}

sf::Vector2f TextLogRenderer::getSize() const
{
	return m_size;
}

void TextLogRenderer::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	m_needsUpdate = false;
}

void TextLogRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!isVisible())
		return;
	auto view = target.getView();
	ensureUpdate();
	states.transform *= getTransform();

	auto s = target.getSize();
	m_view.setViewport(sf::FloatRect(getPosition().x / s.x, getPosition().y / s.y, m_size.x / s.x, m_size.y / s.y));

	target.draw(m_bg, states);

	target.setView(m_view);
	for (auto& item : m_items)
		target.draw(*item);
	target.setView(view);

	target.draw(m_scrollBar, states);
}

} // namespace NovelTea
