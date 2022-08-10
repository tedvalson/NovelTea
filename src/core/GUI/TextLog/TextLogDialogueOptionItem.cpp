#include <NovelTea/GUI/TextLog/TextLogDialogueOptionItem.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

TextLogDialogueOptionItem::TextLogDialogueOptionItem(Context *context, const std::string &text)
	: TextLogItem(context)
	, m_text(context)
{
	TextProperties textProps;
	m_text.setText(text, textProps);

	m_bg.setTexture(AssetManager<sf::Texture>::get("images/button-radius.9.png").get());
	m_bg.setColor(sf::Color(180, 180, 180, 180));
}

void TextLogDialogueOptionItem::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_bg.getColor, m_bg.setColor, 180.f);
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
