#include <NovelTea/GUI/TextLog/TextLogRenderer.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/GUI/TextLog/TextLogDialogueOptionItem.hpp>
#include <NovelTea/GUI/TextLog/TextLogDialogueTextItem.hpp>
#include <NovelTea/GUI/TextLog/TextLogGenericItem.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/System/Err.hpp>

namespace NovelTea
{

TextLogRenderer::TextLogRenderer()
: m_needsUpdate(true)
, m_mousePressed(false)
, m_numLoaded(0)
, m_alpha(255.f)
{
	m_bg.setFillColor(sf::Color(240.f, 240.f, 240.f));
	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setDeceleration(0.9f);
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);
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
	loadItems(5);
	m_scrollBar.update(delta);
	Hideable::update(delta);
	return true;
}

void TextLogRenderer::setScroll(float position)
{
	m_scrollPos = round(position);
	m_view.setCenter({m_size.x / 2.f, -m_scrollAreaSize.y + m_size.y / 2.f - m_scrollPos});
}

float TextLogRenderer::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &TextLogRenderer::getScrollSize()
{
	return m_scrollAreaSize;
}

void TextLogRenderer::repositionItems(float posY, unsigned int startIndex)
{
	auto padding = 5.f;
	auto initialPosY = posY;
	auto w = m_size.x - padding * 2.f;
	for (int i = startIndex; i < m_items.size(); ++i)
	{
		auto& item = m_items[i];
		item->setFontSizeMultiplier(m_fontSizeMultiplier);
		item->setWidth(w);
		posY -= item->getLocalBounds().height;
		item->setPosition({padding, posY});
	}

	m_scrollAreaSize.y = -posY;
	updateScrollbar();
	m_scrollBar.setScroll(m_scrollPos + posY - initialPosY);
}

void TextLogRenderer::loadItems(unsigned int count)
{
	const auto& entries = ActiveGame->getTextLog()->entries();
	count = std::min(count, entries.size() - m_numLoaded);
	if (count <= 0)
		return;

	int startPos = entries.size() - 1 - m_numLoaded;
	int endPos = startPos - count;
	int itemSize = m_items.size();

	for (int i = startPos; i > endPos; --i, ++m_numLoaded)
	{
		TextLogItem* item = nullptr;
		const auto& entry = entries[i];

		if (entry.type == TextLogType::DialogueText) {
			auto text = entry.text;
			if (--i <= endPos)
				break;
			++m_numLoaded;
			item = new TextLogDialogueTextItem(entries[i].text, text);
		} else if (entry.type == TextLogType::DialogueOption) {
			item = new TextLogDialogueOptionItem(entry.text);
		} else {
			item = new TextLogGenericItem(entry.text);
		}

		if (item)
			m_items.emplace_back(item);
	}

	repositionItems(-m_scrollAreaSize.y, itemSize);
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
	m_scrollBar.hide();
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

	float top = -m_scrollAreaSize.y - m_scrollPos;
	float bottom = top + m_size.y;
	bool done = false;
	for (auto& item : m_items)
	{
		auto y = item->getPosition().y;
		if (y < bottom && y + item->getLocalBounds().height > top) {
			done = true;
			item->setAlpha(alpha);
		}
		else if (done)
			break;
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

	m_view.reset(sf::FloatRect(0.f, -size.y, size.x, size.y));

	m_bg.setSize(size);

	m_scrollBar.setPosition(size.x - 4.f, 4.f);
	m_scrollBar.setSize(sf::Vector2f(2, size.y - 8.f));
	m_scrollBar.setScrollAreaSize(sf::Vector2f(size.x, size.y));

	m_size = size;

	if (m_numLoaded == 0)
		loadItems(50);
	else
		repositionItems();

	m_scrollBar.setScroll(-m_scrollAreaSize.y);
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
	float top = -m_scrollAreaSize.y - m_scrollPos;
	float bottom = top + m_size.y;
	bool drawn = false;
	for (auto& item : m_items)
	{
		auto y = item->getPosition().y;
		if (y < bottom && y + item->getLocalBounds().height > top) {
			drawn = true;
			target.draw(*item);
		}
		else if (drawn)
			break;
	}
	target.setView(view);

	target.draw(m_scrollBar, states);
}

} // namespace NovelTea
