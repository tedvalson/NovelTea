#include <NovelTea/MapRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/ProjectData.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Window/Event.hpp>

namespace NovelTea
{

MapRenderer::MapRenderer()
: m_needsUpdate(true)
, m_nameAlpha(255.f)
, m_zoomFactor(3.f)
, m_map(nullptr)
, m_dragging(false)
, m_miniMapMode(true)
, m_modeTransitioning(false)
, m_modeLocked(false)
, m_miniMapRadius(0.f)
{
	auto texture = AssetManager<sf::Texture>::get("images/minimap.png");
	m_spriteCircle.setTexture(*texture, true);

	m_bgShape.setFillColor(sf::Color(210, 210, 210));

	m_buttonClose.getText().setFont(*Proj.getFont(1));
	m_buttonClose.setString(L"\uf00d");
	m_buttonClose.setColor(sf::Color(230, 0 , 0));
	m_buttonClose.setTextColor(sf::Color::White);
	m_buttonClose.onClick([this](){
		setMiniMapMode(true, 0.5f);
	});

	setSize({300.f, 300.f});
	setMiniMapSize({100.f, 100.f});
	setAlpha(255.f);
}

bool MapRenderer::update(float delta)
{
	m_tweenManager.update(delta);
	return Hideable::update(delta);
}

// Return false to stop event propagation (like when not minimap mode)
bool MapRenderer::processEvent(const sf::Event &event)
{
	if (m_modeTransitioning)
		return false;

	if (!m_modeLocked && m_buttonClose.processEvent(event))
		return false;

	if (event.type == sf::Event::MouseButtonPressed) {
		m_lastDragPosition.x = event.mouseButton.x;
		m_lastDragPosition.y = event.mouseButton.y;
		if (m_miniMapMode) {
			m_dragging = getBounds().contains(m_lastDragPosition);
		} else {
			m_dragging = true;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased) {
		if (!m_modeLocked) {
			// If minimap is clicked on, change map mode
			if (m_dragging && m_miniMapMode && getBounds().contains(m_lastDragPosition)) {
				m_dragging = false;
				setMiniMapMode(false, 0.5f);
				return false;
			}
		}
		m_dragging = false;
	}
	else if (event.type == sf::Event::MouseMoved) {
		if (m_dragging) {
			sf::Vector2f pos(event.mouseMove.x, event.mouseMove.y);
			auto diff = (pos - m_lastDragPosition) * m_zoomFactor;
			m_lastDragPosition = pos;

			// Drag map around when not minimap
			if (!m_miniMapMode) {
				m_center -= diff;
				m_needsUpdate = true;
				return false;
			}
			// If drag position leaves minimap, cancel "click"
			else if (!getBounds().contains(m_lastDragPosition))
				m_dragging = false;
		}
	}
	return m_miniMapMode;
}

namespace {
float convertVal(float val, float maxVal)
{
	if (val > 0) {
		if (val < maxVal - 1.f)
			val += 0.5f;
		else
			val = maxVal;
	}
	return val;
}
}

void MapRenderer::setMap(const std::shared_ptr<Map> &map)
{
	m_map = map;
	m_rooms.clear();
	m_paths.clear();
	m_doorways.clear();
	m_roomIdHashmap.clear();
	m_mapSize = {0.f, 0.f};
	if (!map)
		return;

	auto& rooms = map->getRooms();
	auto multiplier = 15.f; // Based on editor grid width Node::snapValue

	for (auto& room : rooms)
	{
		auto shape = new TweenRectangleShape;
		const auto &rect = room->rect;
		float thickness = round(0.1f * multiplier);
		sf::Vector2f vecThickness(thickness, thickness);
		shape->setSize(sf::Vector2f(rect.width, rect.height) * multiplier - vecThickness * 2.f);
		shape->setPosition(sf::Vector2f(rect.left, rect.top) * multiplier + vecThickness);
		shape->setOutlineThickness(thickness);
		shape->setOutlineColor(sf::Color::Black);
		shape->setFillColor(sf::Color::White);

		auto text = new ActiveText;
		TextFormat fmt;
		fmt.size(multiplier);
		fmt.color(sf::Color::Black);
		text->setAlpha(m_nameAlpha);
		text->setFontSizeMultiplier(0.7f);
		text->setText(room->name, fmt);
		while (text->getLocalBounds().width > shape->getSize().x) {
			fmt.size(fmt.size() - 1);
			text->setText(room->name, fmt);
		}
		text->setSize(shape->getSize());
		text->setPosition(sf::Vector2f(rect.left, rect.top) * multiplier);
		text->move({(shape->getSize().x - text->getLocalBounds().width) / 2.f,
				   (shape->getSize().y - text->getLocalBounds().height) / 2.f});

		m_mapSize.x = std::max(m_mapSize.x, shape->getPosition().x + shape->getSize().x);
		m_mapSize.y = std::max(m_mapSize.y, shape->getPosition().y + shape->getSize().y);

		for (auto& roomId : room->roomIds)
			m_roomIdHashmap[roomId].insert(m_rooms.size());

		m_rooms.emplace_back(new Room{room,
									  std::unique_ptr<TweenRectangleShape>(shape),
									  std::unique_ptr<ActiveText>(text),
									  false, false});
	}

	for (auto& conn : map->getConnections())
	{
		auto& rectStart = rooms[conn->roomStart]->rect;
		auto& rectEnd = rooms[conn->roomEnd]->rect;
		sf::FloatRect doorRect;

		if (map->checkForDoor(*conn, doorRect)) {
			auto shape = new TweenRectangleShape;
			shape->setPosition(sf::Vector2f{doorRect.left, doorRect.top} * multiplier);
			shape->setSize(sf::Vector2f{doorRect.width, doorRect.height} * multiplier);
			shape->setFillColor(sf::Color(200, 200, 200));
			m_doorways.emplace_back(new Doorway{
					{conn, m_rooms[conn->roomStart], m_rooms[conn->roomEnd], false},
					std::unique_ptr<TweenRectangleShape>(shape)
			});
		} else {
			sf::Vector2f startPos(convertVal(conn->portStart.x, rectStart.width) + rectStart.left,
								  convertVal(conn->portStart.y, rectStart.height) + rectStart.top);
			sf::Vector2f endPos(convertVal(conn->portEnd.x, rectEnd.width) + rectEnd.left,
								  convertVal(conn->portEnd.y, rectEnd.height) + rectEnd.top);
			auto buffer = new sf::VertexBuffer(sf::PrimitiveType::LineStrip, sf::VertexBuffer::Static);
			sf::Vertex v[2];
			v[0].position = startPos * multiplier;
			v[0].color = sf::Color(100, 100, 100);
			v[1].position = endPos * multiplier;
			v[1].color = sf::Color(100, 100, 100);
			buffer->create(2);
			if (buffer->update(v))
				m_paths.emplace_back(new Path{
						{conn, m_rooms[conn->roomStart], m_rooms[conn->roomEnd], false},
						std::unique_ptr<sf::VertexBuffer>(buffer)
				});
		}
	}

	m_needsUpdate = true;
	reset(0.f);
}

const std::shared_ptr<Map> &MapRenderer::getMap() const
{
	return m_map;
}

void MapRenderer::setActiveRoomId(const std::string &roomId)
{
	if (roomId == m_activeRoomId)
		return;
	m_activeRoomId = roomId;
	reset();
}

const std::string &MapRenderer::getActiveRoomId() const
{
	return m_activeRoomId;
}

void MapRenderer::setMiniMapMode(bool enable, float duration)
{
	if (m_modeTransitioning || m_miniMapMode == enable)
		return;
	m_miniMapMode = enable;
	m_modeTransitioning = true;

	TweenEngine::Tween::mark()
		.delay(duration)
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this](TweenEngine::BaseTween*){
			m_modeTransitioning = false;
		})
		.start(m_tweenManager);

	reset(duration);
}

bool MapRenderer::getMiniMapMode() const
{
	return m_miniMapMode;
}

void MapRenderer::setMiniMapPosition(const sf::Vector2f &position)
{
	m_miniMapPosition = position;
	m_needsUpdate = true;

	m_bounds = {m_miniMapPosition.x, m_miniMapPosition.y, m_miniMapSize.x, m_miniMapSize.y};
	reset(0.f);
}

const sf::Vector2f &MapRenderer::getMiniMapPosition() const
{
	return m_miniMapPosition;
}

void MapRenderer::setMiniMapSize(const sf::Vector2f &size)
{
	m_miniMapSize = size;
	m_needsUpdate = true;

	m_miniMapSize = size;
	m_bounds = {m_miniMapPosition.x, m_miniMapPosition.y, m_miniMapSize.x, m_miniMapSize.y};

	reset(0.f);
}

const sf::Vector2f &MapRenderer::getMiniMapSize() const
{
	return m_miniMapSize;
}

const sf::FloatRect &MapRenderer::getBounds() const
{
	return m_bounds;
}

void MapRenderer::setZoomFactor(float zoomFactor)
{
	m_zoomFactor = zoomFactor;
	m_needsUpdate = true;
}

float MapRenderer::getZoomFactor() const
{
	return m_zoomFactor;
}

void MapRenderer::setCenter(const sf::Vector2f &center)
{
	m_center = center;
	m_needsUpdate = true;
}

const sf::Vector2f &MapRenderer::getCenter() const
{
	return m_center;
}

void MapRenderer::setSize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto wi = std::min(w, h);
	m_size = size;

	m_renderTexture.create(w, h);
	m_renderTexture.setSmooth(true);
	m_renderSprite.setTexture(m_renderTexture.getTexture(), true);

	auto buttonHeight = wi * 0.09f;
	m_buttonClose.getText().setCharacterSize(buttonHeight * 0.9f);
	m_buttonClose.setSize(buttonHeight, buttonHeight);
	m_buttonClose.setPosition(size.x - m_buttonClose.getSize().x * 1.3f, m_buttonClose.getSize().y * 0.3f);

	m_needsUpdate = true;
	reset(0.f);
}

const sf::Vector2f &MapRenderer::getSize() const
{
	return m_size;
}

void MapRenderer::setAlpha(float alpha)
{
	m_alpha = alpha;

	auto color = m_renderSprite.getColor();
	color.a = alpha;
	m_renderSprite.setColor(color);
}

float MapRenderer::getAlpha() const
{
	return m_alpha;
}

// Should be called whenever script engine is used.
// Reevaluates all scripts and resets positioning.
void MapRenderer::reset(float duration)
{
	if (!m_map)
		return;

	for (auto& room : m_rooms)
	{
		room->active = false;
		room->visible = m_map->evalVisibility(room->room);
		room->shape->setFillColor(sf::Color(200, 200, 200));
		TweenEngine::Tween::to(*room->text, ActiveText::ALPHA, duration)
			.target((room->visible && !m_miniMapMode) ? 255.f : 0.f)
			.start(m_tweenManager);
	}

	for (auto& path : m_paths)
		path->connection.visible = m_map->evalVisibility(path->connection.connection);
	for (auto& door : m_doorways)
		door->connection.visible = m_map->evalVisibility(door->connection.connection);

	// These values are for fullscreen map mode
	sf::Vector2f center = m_miniMapPosition + m_miniMapSize / 2.f + (m_mapSize - m_size) / 2.f;
	float zoomFactor = 1.f; // TODO: need reasonable zoom value for all DPIs
	float minimapRadius = std::max(m_size.x, m_size.y);
	float buttonAlpha = 255.f;

	if (!m_activeRoomId.empty())
	{
		auto& indexSet = m_roomIdHashmap[m_activeRoomId];
		for (int i : indexSet) {
			auto& room = m_rooms[i];
			auto& shape = room->shape;
			auto& pos = shape->getPosition();
			auto& size = shape->getSize();
			room->active = true;
			shape->setFillColor(sf::Color::White);
			if (m_miniMapMode) {
				center = sf::Vector2f(pos.x + size.x / 2.f, pos.y + size.y / 2.f);
				zoomFactor = std::max(size.x, size.y) / m_miniMapSize.x * 2.f;
			}
			if (room->visible)
				TweenEngine::Tween::to(*room->text, ActiveText::ALPHA, duration)
					.target(255.f)
					.start(m_tweenManager);
		}
	}

	if (m_miniMapMode) {
		minimapRadius = 0.f;
		buttonAlpha = 0.f;
		if (m_activeRoomId.empty()) {
			center = sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f); // TODO: Fix this?
		}
	}

	TweenEngine::Tween::to(*this, VIEW_CENTER, duration)
		.target(center.x, center.y)
		.start(m_tweenManager);
	TweenEngine::Tween::to(*this, VIEW_ZOOM, duration)
		.target(zoomFactor)
		.start(m_tweenManager);
	TweenEngine::Tween::to(*this, MINIMAP_RADIUS, duration)
		.target(minimapRadius)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonClose, Button::ALPHA, duration)
		.target(buttonAlpha)
		.start(m_tweenManager);

	updateRadius();
}

void MapRenderer::drawToTexture() const
{
	if (!m_map)
		return;
	m_renderTexture.clear(sf::Color::Transparent);
	m_renderTexture.draw(m_bgShape);

	auto origView = m_renderTexture.getView();
	m_renderTexture.setView(m_view);
	for (auto& room : m_rooms)
		if (room->visible)
			m_renderTexture.draw(*room->shape);
	for (auto& path : m_paths)
		if (path->connection.visible)
			m_renderTexture.draw(*path->buffer);
	for (auto& doorway : m_doorways)
		if (doorway->connection.visible)
			m_renderTexture.draw(*doorway->shape);
	for (auto& room : m_rooms)
		m_renderTexture.draw(*room->text);

	m_renderTexture.setView(origView);
	if (m_miniMapMode || m_modeTransitioning)
		m_renderTexture.draw(m_spriteCircle, sf::BlendMultiply);

	m_renderTexture.display();
}

void MapRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!m_map || getAlpha() <= 0.f)
		return;
	if (m_needsUpdate) {
		m_needsUpdate = false;
		updateView();
		drawToTexture();
	}

	target.draw(m_renderSprite);
	if (!m_miniMapMode && !m_modeLocked)
		target.draw(m_buttonClose);
}

void MapRenderer::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case NAME_ALPHA: {
			m_nameAlpha = newValues[0];
			break;
		}
		case VIEW_CENTER:
			setCenter({newValues[0], newValues[1]});
			break;
		case VIEW_ZOOM:
			setZoomFactor(newValues[0]);
			break;
		case MINIMAP_RADIUS:
			setMiniMapRadius(newValues[0]);
			break;
		default:
			Hideable::setValues(tweenType, newValues);
	}
}

int MapRenderer::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case NAME_ALPHA:
			returnValues[0] = m_nameAlpha;
			return 1;
		case VIEW_CENTER:
			returnValues[0] = m_center.x;
			returnValues[1] = m_center.y;
			return 2;
		case VIEW_ZOOM:
			returnValues[0] = getZoomFactor();
			return 1;
		case MINIMAP_RADIUS:
			returnValues[0] = getMiniMapRadius();
			return 1;
		default:
			return Hideable::getValues(tweenType, returnValues);
	}
}

void MapRenderer::setMiniMapRadius(float radius)
{
	if (m_miniMapRadius == radius)
		return;
	m_miniMapRadius = radius;
	m_needsUpdate = true;
	updateRadius();
}

float MapRenderer::getMiniMapRadius() const
{
	return m_miniMapRadius;
}

void MapRenderer::updateRadius()
{
	sf::Vector2f realSize = m_miniMapSize + sf::Vector2f(m_miniMapRadius*2, m_miniMapRadius*2);
	sf::Vector2f realPos = m_miniMapPosition - sf::Vector2f(m_miniMapRadius, m_miniMapRadius);
	auto targetScale = realSize.x / m_spriteCircle.getTexture()->getSize().x;
	m_spriteCircle.setScale(targetScale, targetScale);
	m_bgShape.setSize(realSize);
	m_spriteCircle.setPosition(realPos);
	m_bgShape.setPosition(realPos);
}

void MapRenderer::updateView() const
{
	sf::Vector2f realSize = m_miniMapSize + sf::Vector2f(m_miniMapRadius*2, m_miniMapRadius*2);
	// Rounding correction to avoid rendering artifacts
	realSize -= sf::Vector2f(2, 2);
	sf::Vector2f realPos = m_miniMapPosition - sf::Vector2f(m_miniMapRadius, m_miniMapRadius);
	sf::FloatRect viewport;
	viewport.left = realPos.x / m_size.x;
	viewport.top = realPos.y / m_size.y;
	viewport.width = realSize.x / m_size.x;
	viewport.height = realSize.y / m_size.y;
	m_view.reset({0.f, 0.f, realSize.x, realSize.y});
	m_view.setViewport(viewport);
	m_view.setCenter(m_center);
	m_view.zoom(m_zoomFactor);
}

} // namespace NovelTea
