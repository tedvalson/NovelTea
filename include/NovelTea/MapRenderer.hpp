#ifndef NOVELTEA_MAPRENDERER_HPP
#define NOVELTEA_MAPRENDERER_HPP

#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <memory>
#include <set>

namespace sf {
class Event;
class VertexBuffer;
}

namespace NovelTea
{

class ActiveText;
class Map;

class MapRenderer : public sf::Drawable, public Hideable
{
public:
	static const int NAME_ALPHA  = 12;
	static const int VIEW_ZOOM   = 13;
	static const int VIEW_CENTER = 14;
	static const int MINIMAP_RADIUS = 15;

	struct Room {
		std::unique_ptr<TweenRectangleShape> shape;
		std::unique_ptr<ActiveText> text;
		std::vector<std::string> roomIds;
		std::string script;
		bool visible;
		bool active;
	};

	struct Connection {
		std::unique_ptr<Room>& roomStart;
		std::unique_ptr<Room>& roomEnd;
		std::string script;
		bool visible;
	};

	struct Path {
		Connection connection;
		std::unique_ptr<sf::VertexBuffer> buffer;
	};

	struct Doorway {
		Connection connection;
		std::unique_ptr<TweenRectangleShape> shape;
	};

	MapRenderer();

	bool update(float delta) override;
	bool processEvent(const sf::Event &event);

	void setMap(const std::shared_ptr<Map> &map);
	const std::shared_ptr<Map> &getMap() const;

	void setActiveRoomId(const std::string &roomId);
	const std::string& getActiveRoomId() const;

	void setMiniMapMode(bool enable, float duration = 0.f);
	bool getMiniMapMode() const;
	void setModeLocked(bool locked) { m_modeLocked = locked; }
	bool getModeLocked() const { return m_modeLocked; }

	void setMiniMapPosition(const sf::Vector2f& position);
	const sf::Vector2f& getMiniMapPosition() const;
	void setMiniMapSize(const sf::Vector2f& size);
	const sf::Vector2f& getMiniMapSize() const;

	const sf::FloatRect& getBounds() const;

	void setZoomFactor(float zoomFactor);
	float getZoomFactor() const;

	void setCenter(const sf::Vector2f& center);
	const sf::Vector2f& getCenter() const;

	void setSize(const sf::Vector2f &size);
	const sf::Vector2f &getSize() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void reset(float duration = 1.f);

protected:
	void drawToTexture() const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setValues(int tweenType, float *newValues) override;
	int getValues(int tweenType, float *returnValues) override;

private:
	void setMiniMapRadius(float radius);
	float getMiniMapRadius() const;
	void updateRadius();
	void updateView() const;

private:
	mutable bool m_needsUpdate;
	float m_alpha;
	float m_nameAlpha;
	float m_zoomFactor;
	std::shared_ptr<Map> m_map;

	mutable sf::View m_view;
	sf::Vector2f m_size;
	sf::Vector2f m_center;
	sf::FloatRect m_bounds;

	bool m_dragging;
	sf::Vector2f m_lastDragPosition;

	bool m_miniMapMode;
	bool m_modeTransitioning;
	bool m_modeLocked;
	float m_miniMapRadius;
	sf::Vector2f m_miniMapPosition;
	sf::Vector2f m_miniMapSize;
	sf::Vector2f m_mapPosition;
	sf::Vector2f m_mapSize;

	TweenEngine::TweenManager m_tweenManager;

	std::string m_activeRoomId;

	mutable sf::RenderTexture m_renderTexture;
	TweenSprite m_renderSprite;
	TweenSprite m_spriteCircle;
	sf::RectangleShape m_bgShape;
	Button m_buttonClose;

	std::map<std::string, std::set<int>> m_roomIdHashmap;
	std::vector<std::unique_ptr<Room>> m_rooms;
	std::vector<std::unique_ptr<Path>> m_paths;
	std::vector<std::unique_ptr<Doorway>> m_doorways;
};

} // namespace NovelTea

#endif // NOVELTEA_MAPRENDERER_HPP
