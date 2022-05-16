#include <NovelTea/GUI/TextLog/TextLogDialogueOptionItem.hpp>
#include <NovelTea/AssetManager.hpp>

namespace NovelTea
{

TextLogDialogueOptionItem::TextLogDialogueOptionItem(const std::string &text)
{
	TextFormat fmt;
	m_text.setText(text, fmt);

	m_bg.setTexture(AssetManager<sf::Texture>::get("images/button-radius.9.png").get());
	m_bg.setColor(sf::Color(180, 180, 180, 180));
}

void TextLogDialogueOptionItem::setAlpha(float alpha)
{
	m_text.setAlpha(alpha);
}

void TextLogDialogueOptionItem::setWidth(float width)
{
	m_text.setFontSizeMultiplier(getFontSizeMultiplier());
	m_text.setSize({width, 0.f}); // Height is not relevant
	m_bg.setSize({m_text.getLocalBounds().width, m_text.getLocalBounds().height});

	m_bounds = m_text.getLocalBounds();
}

void TextLogDialogueOptionItem::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_bg, states);
	target.draw(m_text, states);
}

} // namespace NovelTea
