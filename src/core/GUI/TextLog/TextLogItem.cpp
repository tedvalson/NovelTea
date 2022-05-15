#include <NovelTea/GUI/TextLog/TextLogItem.hpp>

namespace NovelTea
{

TextLogItem::TextLogItem()
: m_fontSizeMultiplier(1.f)
{

}

void TextLogItem::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_fontSizeMultiplier = fontSizeMultiplier;
}

float TextLogItem::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
}

sf::FloatRect TextLogItem::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect TextLogItem::getGlobalBounds() const
{
	return getTransform().transformRect(m_bounds);
}

} // namespace NovelTea
