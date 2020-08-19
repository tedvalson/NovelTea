#include <cmath>
#include <NovelTea/GUI/Button.hpp>
#include <NovelTea/ProjectData.hpp>

namespace NovelTea
{

Button::Button()
: m_backgroundColor(sf::Color::White)
, m_needsUpdate(true)
, m_autoSize(true)
, m_active(false)
{
	m_text.setFont(*Proj.getFont(0));
}


void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	NinePatch::draw(target, states);

	states.transform *= getTransform();

	if (!m_text.getString().isEmpty())
		target.draw(m_text, states);
}


void Button::setTextOffset(const sf::Vector2f& offset)
{
	m_textOffset = offset;
	m_needsUpdate = true;
}


const sf::Vector2f& Button::getTextOffset() const
{
	return m_textOffset;
}


void Button::setContentSize(const sf::Vector2f& size)
{
	NinePatch::setContentSize(size);
	m_autoSize = false;
}


void Button::setContentSize(float width, float height)
{
	NinePatch::setContentSize(width, height);
	m_autoSize = false;
}


const sf::Vector2f& Button::getContentSize() const
{
	ensureUpdate();
	return NinePatch::getContentSize();
}


const sf::Vector2f& Button::getSize() const
{
	ensureUpdate();
	return m_size;
}


void Button::setString(const sf::String& string)
{
	m_text.setString(string);
	m_needsUpdate = true;
}


const sf::String& Button::getString() const
{
	return m_text.getString();
}


void Button::setTextColor(const sf::Color& color)
{
	m_textColor = color;
	m_needsUpdate = true;
}


const sf::Color& Button::getTextColor() const
{
	return m_textColor;
}


void Button::setTextActiveColor(const sf::Color& color)
{
	m_textActiveColor = color;
	m_needsUpdate = true;
}


const sf::Color& Button::getTextActiveColor() const
{
	return m_textActiveColor;
}


void Button::setColor(const sf::Color& color)
{
	m_backgroundColor = color;
	m_needsUpdate = true;
}


const sf::Color& Button::getColor() const
{
	return m_backgroundColor;
}


void Button::setActiveColor(const sf::Color& color)
{
	m_backgroundActiveColor = color;
	m_needsUpdate = true;
}


const sf::Color& Button::getActiveColor() const
{
	return m_backgroundActiveColor;
}


void Button::autoSize()
{
	m_autoSize = true;
	m_needsUpdate = true;
}


bool Button::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (m_rect.contains(event.touch.x, event.touch.y))
		{
			m_active = true;
			m_needsUpdate = true;
			return false;
		}
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (m_active) {
			m_active = false;
			m_needsUpdate = true;
			if (m_clickFunction)
				m_clickFunction();
			return false;
		}
	}

	if (event.type == sf::Event::MouseMoved)
	{
		if (m_active) {
			if (!m_rect.contains(event.touch.x, event.touch.y)) {
				m_active = false;
				m_needsUpdate = true;
			}
			return false;
		}
	}

	return true;
}


void Button::ensureUpdate() const
{
	if (m_needsUpdate)
	{
		if (m_autoSize)
			NinePatch::setContentSize(sf::Vector2f(m_text.getLocalBounds().width, m_text.getLocalBounds().height));

		sf::FloatRect textBounds = m_text.getLocalBounds();
		sf::FloatRect padding = getPadding();
		sf::Vector2f contentSize = NinePatch::getContentSize();

		if (m_autoSize && getTexture()) {
			m_size.x = contentSize.x + getTexture()->getSize().x - padding.width;
			m_size.y = contentSize.y + getTexture()->getSize().y - padding.height;
		} else {
			m_size = NinePatch::getSize();
		}

		m_text.setOrigin(round(textBounds.left + textBounds.width/2),
						 round(textBounds.top + textBounds.height/2));
		m_text.setPosition(round(padding.left + contentSize.x/2 + m_textOffset.x),
						   round(padding.top + contentSize.y/2 + m_textOffset.y));

		if (m_active) {
			NinePatch::setColor(m_backgroundActiveColor);
			m_text.setFillColor(m_textActiveColor);
		} else {
			NinePatch::setColor(m_backgroundColor);
			m_text.setFillColor(m_textColor);
		}

		m_needsUpdate = false;
	}

	m_rect.left = getPosition().x;
	m_rect.top = getPosition().y;
	m_rect.width = m_size.x;
	m_rect.height = m_size.y;
}


void Button::onClick(const std::function<void()>& callback)
{
	m_clickFunction = callback;
}

void Button::click()
{
	if (m_clickFunction)
		m_clickFunction();
}


int Button::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case COLOR_RGB: {
			sf::Color color = getColor();
			returnValues[0] = color.r;
			returnValues[1] = color.g;
			returnValues[2] = color.b;
			return 3;
		}
		case COLOR_ALPHA: returnValues[0] = getColor().a; return 1;
		case CONTENT_X: returnValues[0] = getContentSize().x; return 1;
		case TEXTCOLOR_ALPHA: returnValues[0] = getTextColor().a; return 1;
		default:
			return TweenTransformable::getValues(tweenType, returnValues);
	}
}


void Button::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case COLOR_RGB: {
			sf::Color color;
			color.r = std::max(std::min(newValues[0], 255.f), 0.f);
			color.g = std::max(std::min(newValues[1], 255.f), 0.f);
			color.b = std::max(std::min(newValues[2], 255.f), 0.f);
			color.a = getColor().a;
			setColor(color);
			break;
		}
		case COLOR_ALPHA: {
			sf::Color color = getColor();
			color.a = std::max(std::min(newValues[0], 255.f), 0.f);
			setColor(color);
			break;
		}
		case CONTENT_X: setContentSize(sf::Vector2f(newValues[0], getContentSize().y)); break;
		case TEXTCOLOR_ALPHA: {
			sf::Color color = getTextColor();
			color.a = std::max(std::min(newValues[0], 255.f), 0.f);
			setTextColor(color);
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
			break;
	}
}


sf::Text &Button::getText() const
{
	m_needsUpdate = true;
	return m_text;
}


} // namespace NovelTea
