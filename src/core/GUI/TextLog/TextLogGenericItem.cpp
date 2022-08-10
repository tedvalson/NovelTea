#include <NovelTea/GUI/TextLog/TextLogGenericItem.hpp>
#include <NovelTea/TextTypes.hpp>

namespace NovelTea
{

TextLogGenericItem::TextLogGenericItem(Context *context, const std::string &text)
	: TextLogItem(context)
	, m_text(context)
{
	TextProperties textProps;
	m_text.setText(text, textProps);
}

void TextLogGenericItem::setAlpha(float alpha)
{
	m_text.setAlpha(alpha);
}

void TextLogGenericItem::setWidth(float width)
{
	m_text.setFontSizeMultiplier(getFontSizeMultiplier());
	m_text.setSize({width, 0.f}); // Height is not relevant
	m_bounds = m_text.getLocalBounds();
}

void TextLogGenericItem::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_text, states);
}

} // namespace NovelTea
