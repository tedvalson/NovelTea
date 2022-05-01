#include <NovelTea/MapRenderer.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/ProjectData.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Window/Event.hpp>

namespace NovelTea
{

MapRenderer::MapRenderer()
: m_dragging(false)
{
	setZoomFactor(0.1f);
}

bool MapRenderer::update(float delta)
{
	return Hideable::update(delta);
}

bool MapRenderer::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::MouseButtonPressed) {
		m_dragging = true;
		m_lastDragPosition.x = event.mouseButton.x;
		m_lastDragPosition.y = event.mouseButton.y;
	}
	else if (event.type == sf::Event::MouseButtonReleased) {
		m_dragging = false;
	}
	else if (event.type == sf::Event::MouseMoved) {
		if (m_dragging) {
			sf::Vector2f pos(event.mouseMove.x, event.mouseMove.y);
			const auto& center = m_view.getCenter();
			auto diff = (pos - m_lastDragPosition) * m_zoomFactor;
			m_view.setCenter(center - diff);
			m_lastDragPosition = pos;
		}
	}
	return true;
}

void MapRenderer::setMap(const std::shared_ptr<Map> &map)
{
	m_map = map;
	m_texts.clear();
	m_roomShapes.clear();
	m_pathShapes.clear();

	auto& font = *Proj.getFont(0);

	for (auto& room : map->getRooms())
	{
		auto shape = new sf::RectangleShape;
		const auto &rect = room->rect;
		shape->setSize(sf::Vector2f(rect.width, rect.height));
		shape->setPosition(sf::Vector2f(rect.left, rect.top));
		shape->setOutlineThickness(0.1f);
		shape->setOutlineColor(sf::Color::Black);
		shape->setFillColor(sf::Color::White);
		m_roomShapes.emplace_back(shape);

		auto text = new TweenText;
		text->setFont(font);
		text->setString(room->name);
		text->setCharacterSize(12);
		text->setPosition(sf::Vector2f(rect.left, rect.top));
		text->setFillColor(sf::Color::Black);
		m_texts.emplace_back(text);
	}
	for (auto& path : map->getPaths())
	{
		auto shape = new sf::VertexBuffer(sf::PrimitiveType::LineStrip, sf::VertexBuffer::Static);
		sf::Vertex v[2];
		v[0].position = sf::Vector2f(path->start);
//		v[0].color = sf::Color(100, 100, 100);
		v[0].color = sf::Color::Blue;
		v[1].position = sf::Vector2f(path->end);
//		v[1].color = sf::Color(100, 100, 100);
		v[1].color = sf::Color::Red;
		shape->create(2);
		if (shape->update(v))
			m_pathShapes.emplace_back(shape);
	}
}

const std::shared_ptr<Map> &MapRenderer::getMap() const
{
	return m_map;
}

void MapRenderer::setZoomFactor(float zoomFactor)
{
	m_zoomFactor = zoomFactor;
	updateView();
}

float MapRenderer::getZoomFactor() const
{
	return m_zoomFactor;
}

void MapRenderer::setSize(const sf::Vector2f &size)
{
	m_size = size;
	updateView();
}

const sf::Vector2f &MapRenderer::getSize() const
{
	return m_size;
}

void MapRenderer::setAlpha(float alpha)
{
	m_alpha = alpha;
}

float MapRenderer::getAlpha() const
{
	return m_alpha;
}

void MapRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	auto origView = target.getView();
	target.setView(m_view);
	for (auto& s : m_roomShapes)
		target.draw(*s, states);
	for (auto& s : m_pathShapes)
		target.draw(*s, states);
	for (auto& text : m_texts)
		target.draw(*text, states);
	target.setView(origView);
}

void MapRenderer::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case NAME_ALPHA: {
			m_nameAlpha = newValues[0];
			for (auto& text : m_texts) {
				//
			}
			break;
		}
		case ZOOM:
			setZoomFactor(newValues[0]);
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
		case ZOOM:
			returnValues[0] = getZoomFactor();
			return 1;
		default:
			return Hideable::getValues(tweenType, returnValues);
	}
}

void MapRenderer::updateView()
{
	m_view.reset({0.f, 0.f, m_size.x, m_size.y});
	m_view.setViewport({0.f, 0.f, 1.f, 1.f});
	m_view.setCenter({10.f, 15.f});
	m_view.zoom(m_zoomFactor);
}

} // namespace NovelTea
