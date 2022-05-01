#ifndef NOVELTEA_MAPRENDERER_HPP
#define NOVELTEA_MAPRENDERER_HPP

#include <NovelTea/GUI/Hideable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <memory>

namespace sf {
class Event;
class VertexBuffer;
}

namespace NovelTea
{

class Map;

class MapRenderer : public sf::Drawable, public Hideable
{
public:
	static const int NAME_ALPHA = 12;
	static const int ZOOM = 13;

	MapRenderer();

	bool update(float delta) override;
	bool processEvent(const sf::Event &event);

	void setMap(const std::shared_ptr<Map> &map);
	const std::shared_ptr<Map> &getMap() const;

	void setZoomFactor(float zoomFactor);
	float getZoomFactor() const;

	void setSize(const sf::Vector2f &size);
	const sf::Vector2f &getSize() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setValues(int tweenType, float *newValues) override;
	int getValues(int tweenType, float *returnValues) override;

private:
	void updateView();

private:
	float m_alpha;
	float m_nameAlpha;
	float m_zoomFactor;
	std::shared_ptr<Map> m_map;

	sf::Vector2f m_size;
	sf::View m_view;

	bool m_dragging;
	sf::Vector2f m_lastDragPosition;

	std::vector<std::unique_ptr<TweenText>> m_texts;
	std::vector<std::unique_ptr<sf::RectangleShape>> m_roomShapes;
	std::vector<std::unique_ptr<sf::VertexBuffer>> m_pathShapes;
};

} // namespace NovelTea

#endif // NOVELTEA_MAPRENDERER_HPP
