#include "Node.hpp"

#include <QtCore/QObject>

#include <cmath>
#include <utility>
#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"

unsigned int Node::borderThickness = 15;
unsigned int Node::snapValue = 15;

Node::Node()
: _uid(QUuid::createUuid())
, _nodeGraphicsObject(nullptr)
, _name("Room Name")
, _width(150)
, _height(150)
, _hovered(false)
, _draggingPos(-1000, -1000)
, _reaction(NOT_REACTING)
, _resizing(false)
, _style(NovelTea::RoomStyle::SolidBorder)
{
	setMinHeight(0);
	setMinWidth(0);
}

Node::~Node() = default;

QUuid Node::id() const { return _uid; }

void Node::reactToPossibleConnection(QPointF const& scenePoint)
{
	QTransform const t = _nodeGraphicsObject->sceneTransform();
	QPointF p = t.inverted().map(scenePoint);

	setDraggingPosition(p);

	_nodeGraphicsObject->update();

	QPoint port;
	if (checkHitScenePoint(port, p, t))
		setReaction(Node::REACTING, port);
}

void Node::resetReactionToConnection()
{
	setReaction(Node::NOT_REACTING);
	_nodeGraphicsObject->update();
}

NodeGraphicsObject const& Node::nodeGraphicsObject() const
{
	return *_nodeGraphicsObject.get();
}

NodeGraphicsObject& Node::nodeGraphicsObject()
{
	return *_nodeGraphicsObject.get();
}

void Node::setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics)
{
	_nodeGraphicsObject = std::move(graphics);
}

void Node::recalculateNodeGeometry()
{
	auto const& entries = getEntries();
	int maxX = 0;
	int maxY = 0;
	for (auto& c : entries) {
		QPoint p = c.first;
		auto horizontalWall = onHorizontalWall(p);
		auto verticalWall = onVerticalWall(p);
		if (horizontalWall && !verticalWall)
			maxX = std::max(maxX, p.x());
		else if (verticalWall && !horizontalWall)
			maxY = std::max(maxY, p.y());
	}
	maxX += 2;
	maxY += 2;
	setMinWidth(maxX * Node::snapValue);
	setMinHeight(maxY * Node::snapValue);
}

void Node::setName(const QString &name)
{
	_name = name;
}

void Node::onNodeSizeUpdated()
{
	for (auto& pair : getEntries()) {
		Connection* conn = pair.second;
		conn->getConnectionGraphicsObject().move();
	}
}

void Node::setHeight(unsigned int h)
{
	_height = h - (h % snapValue);
	if (_height < _minHeight)
		_height = _minHeight;
}

void Node::setWidth(unsigned int w)
{
	_width = w - (w % snapValue);
	if (_width < _minWidth)
		_width = _minWidth;
}

void Node::setMinHeight(unsigned int h)
{
	_minHeight = std::max(h, snapValue * 5);
}

void Node::setMinWidth(unsigned int w)
{
	_minWidth = std::max(w, snapValue * 5);
}

bool Node::onHorizontalWall(const QPoint& p)
{
	int h = height() / snapValue;
	return (p.y() == 0 || p.y() == h - 1);
}

bool Node::onVerticalWall(const QPoint& p)
{
	int w = width() / snapValue;
	return (p.x() == 0 || p.x() == w - 1);
}

QRectF Node::boundingRect() const { return QRectF(0.f, 0.f, _width, _height); }

QPointF Node::portScenePosition(QPoint portPoint) const
{
	float length = snapValue;
	QPointF result(length * portPoint.x() + length / 2.f,
				   length * portPoint.y() + length / 2.f);
	return nodeGraphicsObject().sceneTransform().map(result);
}

bool Node::checkHitScenePoint(QPoint& result, QPointF point,
							  const QTransform& t) const
{
	auto length = snapValue;
	auto inv = t.inverted();
	QPointF p = inv.map(point);
	QRectF deadZone(length, length, width() - length * 2,
					height() - length * 2);
	if (deadZone.contains(p))
		return false;
	result = QPoint(p.x() / snapValue, p.y() / snapValue);
	return true;
}

float distance(const QPointF& p1, const QPointF& p2)
{
	auto dx = p1.x() - p2.x();
	auto dy = p1.y() - p2.y();
	return std::sqrt(dx * dx + dy * dy);
}

QPointF Node::snapToPortPoint(const QPointF& pos, const QTransform& t) const
{
	QPointF result;
	float minDist = std::max(_width, _height);
	QPointF p = t.inverted().map(pos);

	QRectF deadZone(snapValue, snapValue, width() - snapValue * 2,
					height() - snapValue * 2);

	for (float x = 0.5f * snapValue; x < _width; x += snapValue)
		for (float y = 0.5f * snapValue; y < _height; y += snapValue) {
			QPointF pp(x, y);
			if (deadZone.contains(pp))
				continue;
			float dist = distance(pp, p);
			if (dist < minDist) {
				minDist = dist;
				result = pp;
			}
		}

	return result;
}

QRect Node::resizeRect() const
{
	unsigned int rectSize = borderThickness;

	return QRect(_width - rectSize - borderThickness,
				 _height - rectSize - borderThickness, rectSize, rectSize);
}

std::vector<Node::Connections> const& Node::getEntries() const
{
	return _connections;
}

std::vector<Node::Connections>& Node::getEntries() { return _connections; }

Connection* Node::connection(QPoint portPoint) const
{
	for (auto& c : _connections)
		if (c.first == portPoint)
			return c.second;
	return nullptr;
}

void Node::setConnection(QPoint portPoint, Connection& connection)
{
	for (auto& c : _connections)
		if (c.first == portPoint) {
			c.second = &connection;
			return;
		}

	_connections.emplace_back(portPoint, &connection);
}

void Node::eraseConnection(QPoint portPoint)
{
	for (auto it = _connections.begin(); it != _connections.end(); ++it)
		if (it->first == portPoint) {
			_connections.erase(it);
			return;
		}
}

void Node::setReaction(Node::ReactToConnectionState reaction,
					   const QPoint& portPoint)
{
	_reaction = reaction;
	_reactingPoint = portPoint;
}

Node::ReactToConnectionState Node::reaction() const { return _reaction; }

const QPoint& Node::reactingPortPoint() const { return _reactingPoint; }

bool Node::isReacting() const { return _reaction == REACTING; }

void Node::setResizing(bool resizing) { _resizing = resizing; }

bool Node::resizing() const { return _resizing; }
