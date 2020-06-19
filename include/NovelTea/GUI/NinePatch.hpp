#ifndef NOVELTEA_NINEPATCH_HPP
#define NOVELTEA_NINEPATCH_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace NovelTea
{

class NinePatch : public sf::Drawable, public sf::Transformable {
public:
	NinePatch();
	~NinePatch();

	void setTexture(const sf::Texture *texture);
	const sf::Texture* getTexture() const;

	void setContentSize(const sf::Vector2f& size) const;
	void setContentSize(float width, float height) const;
	void setSize(const sf::Vector2f& size) const;
	void setSize(float width, float height) const;
	const sf::Vector2f& getContentSize() const;
	sf::Vector2f getSize() const;

	void setPadding(const sf::FloatRect& padding);
	void setPadding(float left, float top, float width, float height);
	const sf::FloatRect& getPadding() const;

	void setColor(const sf::Color& color) const;
	const sf::Color& getColor() const;

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void ensureUpdate() const;
	void updateRegions() const;
	void updateVertices() const;

private:
	enum ExpandType {
		Stretch,
		Repeat,
	};
	enum RegionType {
		Static,
		DynamicWidth,
		DynamicHeight,
		DynamicBoth,
	};
	struct Region {
		RegionType type;
		sf::FloatRect rect;

	};

	const sf::Texture *m_texture;
	mutable sf::Color m_color;
	mutable sf::Vector2f m_contentSize;
	mutable sf::Vector2f m_size;
	mutable std::vector<Region> m_regions;
	mutable sf::VertexArray m_vertices;
	mutable sf::FloatRect m_padding;

	// Used for internal calculation
	mutable float m_staticWidth;
	mutable float m_staticHeight;
	mutable float m_dynamicWidth;
	mutable float m_dynamicHeight;

	mutable bool m_needsUpdate;
};

} // namespace NovelTea

#endif // NOVELTEA_NINEPATCH_HPP
