#include <NovelTea/GUI/TextLog/TextLogDialogueTextItem.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

TextLogDialogueTextItem::TextLogDialogueTextItem(Context *context, const std::string &name, const std::string &text)
	: TextLogItem(context)
	, m_text(context)
	, m_textName(context)
{
	m_hasName = !name.empty();
	TextProperties textProps;
	m_text.setText(text, textProps);

	auto texture = AssetManager<sf::Texture>::get("images/button-radius.9.png").get();
	m_bg.setTexture(texture);
	m_bg.setColor(sf::Color(220, 220, 220));

	if (m_hasName) {
		m_bgName.setTexture(texture);
		m_bgName.setColor(sf::Color::White);
		m_textName.setText(name, textProps);
	}
}

void TextLogDialogueTextItem::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_bg.getColor, m_bg.setColor, 255.f);

	m_text.setAlpha(alpha);
	if (m_hasName) {
		m_textName.setAlpha(alpha);
		SET_ALPHA(m_bgName.getColor, m_bgName.setColor, 255.f);
	}
}

void TextLogDialogueTextItem::setWidth(float width)
{
	const auto padding = 4.f;
	const auto paddingName = 4.f;

	if (m_hasName) {
		m_textName.setFontSizeMultiplier(getFontSizeMultiplier());
		m_textName.setSize({width, 0.f});
		m_textName.setPosition({paddingName, paddingName});

		auto nameBounds = m_textName.getLocalBounds();
		m_bgName.setSize({nameBounds.width + paddingName * 2.f, nameBounds.height + paddingName * 2.f});
	}

	m_text.setFontSizeMultiplier(getFontSizeMultiplier());
	m_text.setSize({width - padding * 2.f, 0.f}); // Height is not relevant
	m_text.setPosition({padding, padding});
	if (m_hasName)
		m_text.setCursorStart({m_bgName.getSize().x, 0.f});

	m_bounds = {0.f, 0.f, width, m_text.getLocalBounds().height + padding * 2.f};
	m_bg.setSize({m_bounds.width, m_bounds.height});
}

void TextLogDialogueTextItem::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	if (m_hasName) {
		target.draw(m_bgName, states);
		target.draw(m_textName, states);
	}
	target.draw(m_text, states);
}

} // namespace NovelTea
