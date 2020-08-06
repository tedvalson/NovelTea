#ifndef NOVELTEA_BUTTON_HPP
#define NOVELTEA_BUTTON_HPP

#include <functional>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include "NinePatch.hpp"
#include <NovelTea/TweenObjects.hpp>

namespace NovelTea {

class Button : public TweenNinePatch {
public:
	static const int COLOR_RGB   = 11;
	static const int COLOR_ALPHA = 12;
	static const int CONTENT_X   = 13;
	static const int TEXTCOLOR_ALPHA = 14;

	Button();

	bool processEvent(const sf::Event& event);

	void setTextOffset(const sf::Vector2f& offset);
	const sf::Vector2f& getTextOffset() const;

	void autoSize();
	void setContentSize(const sf::Vector2f& size);
	void setContentSize(float width, float height);
	const sf::Vector2f& getContentSize() const;
	const sf::Vector2f& getSize() const;

	void setString(const sf::String& string);
	const sf::String& getString() const;

	sf::Text& getText() const;

	void setTextColor(const sf::Color& color);
	const sf::Color& getTextColor() const;

	void setTextActiveColor(const sf::Color& color);
	const sf::Color& getTextActiveColor() const;

	void setColor(const sf::Color& color);
	const sf::Color& getColor() const;

	void setActiveColor(const sf::Color& color);
	const sf::Color& getActiveColor() const;

	void onClick(const std::function<void()>& callback);

protected:
	virtual int getValues(int tweenType, float *returnValues);
	virtual void setValues(int tweenType, float *newValues);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void ensureUpdate() const;

private:
	mutable sf::Vector2f m_size;
	mutable sf::FloatRect m_rect;
	mutable sf::Text m_text;

	sf::Vector2f m_textOffset;
	sf::Color m_textColor;
	sf::Color m_textActiveColor;

	sf::Color m_backgroundColor;
	sf::Color m_backgroundActiveColor;

	std::function<void()> m_clickFunction;

	mutable bool m_needsUpdate;
	bool m_autoSize;
	bool m_active;
};

} // namespace NovelTea

#endif // NOVELTEA_BUTTON_HPP
