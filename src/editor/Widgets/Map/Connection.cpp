#include "Connection.hpp"

#include <cmath>
#include <utility>

#include <QtWidgets/QtWidgets>
#include <QtGlobal>

#include "Node.hpp"
#include "FlowScene.hpp"
#include "FlowView.hpp"
#include "MapStyle.hpp"

#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"

Connection::Connection(Node& node, QPoint portPoint)
: _uid(QUuid::createUuid())
, _startNode(&node)
, _startPortPoint(portPoint)
, _start(0, 0)
, _end(0, 0)
, _lineWidth(3.0)
, _hovered(false)
, _lastHoveredNode(nullptr)
{
	setNodeToPort(node, true, portPoint);
}

Connection::Connection(Node& nodeStart, QPoint portPointStart, Node& nodeEnd,
					   QPoint portPointEnd)
: Connection(nodeStart, portPointStart)
{
	_endNode = &nodeEnd;
	_endPortPoint = portPointEnd;
	setNodeToPort(nodeStart, true, portPointStart);
	setNodeToPort(nodeEnd, false, portPointEnd);
}

Connection::~Connection()
{
	if (complete())
		connectionMadeIncomplete(*this);

	if (_startNode) {
		_startNode->nodeGraphicsObject().update();
	}

	if (_endNode) {
		_endNode->nodeGraphicsObject().update();
	}

	resetLastHoveredNode();
}

QUuid Connection::id() const { return _uid; }

bool Connection::complete() const
{
	return _startNode != nullptr && _endNode != nullptr;
}

void Connection::setGraphicsObject(
	std::unique_ptr<ConnectionGraphicsObject>&& graphics)
{
	_connectionGraphicsObject = std::move(graphics);

	// This function is only called when the ConnectionGraphicsObject
	// is newly created. At this moment both end coordinates are (0, 0)
	// in Connection G.O. coordinates. The position of the whole
	// Connection G. O. in scene coordinate system is also (0, 0).
	// By moving the whole object to the Node Port position
	// we position both connection ends correctly.

	if (!complete())
	{
		QPoint attachedPoint = getPortPoint(true);

		auto node = getNode(true);

		QTransform nodeSceneTransform =
			node->nodeGraphicsObject().sceneTransform();

		QPointF pos =
			node->portScenePosition(attachedPoint, nodeSceneTransform);

		_connectionGraphicsObject->setPos(pos);
	}

	_connectionGraphicsObject->move();
}

QPoint Connection::getPortPoint(bool startNode) const
{
	return (startNode) ? _startPortPoint : _endPortPoint;
}

void Connection::setNodeToPort(Node& node, bool startNode, QPoint portPoint)
{
	bool wasIncomplete = !complete();
	auto& nodeWeak = getNode(startNode);

	nodeWeak = &node;

	if (startNode)
		_startPortPoint = portPoint;
	else
		_endPortPoint = portPoint;

	updated(*this);
	if (complete() && wasIncomplete) {
		connectionCompleted(*this);
	}
}

void Connection::removeFromNodes() const
{
	if (_startNode)
		_startNode->eraseConnection(_startPortPoint);

	if (_endNode)
		_endNode->eraseConnection(_endPortPoint);
}

ConnectionGraphicsObject& Connection::getConnectionGraphicsObject() const
{
	return *_connectionGraphicsObject;
}

Node* Connection::getNode(bool startNode) const
{
	return (startNode) ? _startNode : _endNode;
}

Node*& Connection::getNode(bool startNode)
{
	return (startNode) ? _startNode : _endNode;
}

bool Connection::startNodeEmpty() const { return _startNode == nullptr; }

bool Connection::endNodeEmpty() const { return _endNode == nullptr; }

void Connection::clearNode(bool startNode)
{
	if (complete()) {
		connectionMadeIncomplete(*this);
	}

	getNode(startNode) = nullptr;
}


QPointF const& Connection::getEndPoint(bool startNode) const
{
	return (startNode) ? _start : _end;
}

void Connection::setEndPoint(bool startNode, QPointF const& point)
{
	if (startNode)
		_start = point;
	else
		_end = point;
}

void Connection::moveEndPoint(bool startNode, QPointF const& offset)
{
	if (startNode)
		_start += offset;
	else
		_end += offset;
}

QRectF Connection::boundingRect() const
{
	auto points = pointsC1C2();
	QRectF basicRect = QRectF(_end, _start).normalized();
	QRectF c1c2Rect = QRectF(points.first, points.second).normalized();
	auto const& style = MapStyle::instance();
	float const diam = style.ConnectionPointDiameter;
	QPointF const cornerOffset(diam, diam);

	QRectF commonRect = basicRect.united(c1c2Rect);
	commonRect.setTopLeft(commonRect.topLeft() - cornerOffset);
	commonRect.setBottomRight(commonRect.bottomRight() + 2 * cornerOffset);

	return commonRect;
}

std::pair<QPointF, QPointF> Connection::pointsC1C2() const
{
	const double defaultOffset = 200;
	double xDistance = _start.x() - _end.x();
	double horizontalOffset = qMin(defaultOffset, std::abs(xDistance));
	double verticalOffset = 0;
	double ratioX = 0.5;

	if (xDistance <= 0) {
		double yDistance = _start.y() - _end.y() + 20;
		double vector = yDistance < 0 ? -1.0 : 1.0;

		verticalOffset =
			qMin(defaultOffset, std::abs(yDistance)) * vector;

		ratioX = 1.0;
	}

	horizontalOffset *= ratioX;

	QPointF c1(_end.x() + horizontalOffset, _end.y() + verticalOffset);
	QPointF c2(_start.x() - horizontalOffset, _start.y() - verticalOffset);

	return std::make_pair(c1, c2);
}


void Connection::interactWithNode(Node* node)
{
	if (node) {
		_lastHoveredNode = node;
	} else {
		resetLastHoveredNode();
	}
}

void Connection::setLastHoveredNode(Node* node)
{
	_lastHoveredNode = node;
}

void Connection::resetLastHoveredNode()
{
	if (_lastHoveredNode)
		_lastHoveredNode->resetReactionToConnection();
	_lastHoveredNode = nullptr;
}
