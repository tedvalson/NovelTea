#ifndef NOVELTEA_TEXTLOGITEM_HPP
#define NOVELTEA_TEXTLOGITEM_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/TweenObjects.hpp>

namespace NovelTea
{

class TextLogItem : public ContextObject, public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	TextLogItem(Context *context);

	virtual void setAlpha(float alpha) = 0;
	virtual void setWidth(float width) = 0;

	void setFontSizeMultiplier(float fontSizeMultiplier);
	float getFontSizeMultiplier() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

protected:
	sf::FloatRect m_bounds;
	float m_fontSizeMultiplier;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOGITEM_HPP
