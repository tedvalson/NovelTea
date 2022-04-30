#include "NodeConnectionInteraction.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"
#include "FlowScene.hpp"

NodeConnectionInteraction::NodeConnectionInteraction(Node& node,
													 Connection& connection,
													 FlowScene& scene)
: _node(&node)
, _connection(&connection)
, _scene(&scene)
{
}

bool NodeConnectionInteraction::canConnect(QPoint& portPoint) const
{
	if (_connection->complete())
		return false;

	// 1.5) Forbid connecting the node to itself
	Node* node = _connection->getNode(!_connection->startNodeEmpty());
	if (node == _node)
		return false;

	// 2) connection point is on top of the node port

	QPointF connectionPoint =
		connectionEndScenePosition(_connection->startNodeEmpty());

	portPoint = nodePortPointUnderScenePoint(connectionPoint);

	if (portPoint.x() < 0)
		return false;

	// port should be empty
	if (!nodePortIsEmpty(portPoint))
		return false;

	return true;
}

bool NodeConnectionInteraction::tryConnect() const
{
	QPoint portPoint;

	if (!canConnect(portPoint))
		return false;

	_node->setConnection(portPoint, *_connection);

	// 3) Assign Connection to empty port in NodeState
	// The port is not longer required after this function
	_connection->setNodeToPort(*_node, _connection->startNodeEmpty(),
							   portPoint);  // TODO: check
	_node->nodeGraphicsObject().moveConnections();
	_node->recalculateNodeGeometry();
	return true;
}

bool NodeConnectionInteraction::moveConnection(const QPoint& newPoint)
{
	bool startNode = _connection->getNode(true) == _node;
	auto currentPoint = _connection->getPortPoint(startNode);
	if (currentPoint == newPoint)
		return true;
	if (!nodePortIsEmpty(newPoint))
		return false;
	_node->eraseConnection(currentPoint);
	_node->setConnection(newPoint, *_connection);
	_connection->setNodeToPort(*_node, startNode, newPoint);
	_node->recalculateNodeGeometry();
	return true;
}

/// 1) Node and Connection should be already connected
/// 2) If so, clear Connection entry in the NodeState
/// 3) Set Connection end to 'requiring a port'
bool NodeConnectionInteraction::disconnect(bool startNode) const
{
	QPoint portPoint = _connection->getPortPoint(startNode);

	_node->eraseConnection(portPoint);
	_connection->clearNode(startNode);

	_connection->getConnectionGraphicsObject().grabMouse();

	_node->recalculateNodeGeometry();
	return true;
}

bool NodeConnectionInteraction::disconnect(Node* node) const
{
	return disconnect(_connection->getNode(true) == node);
}

// ------------------ util functions below

QPointF NodeConnectionInteraction::connectionEndScenePosition(
	bool startNode) const
{
	auto& go = _connection->getConnectionGraphicsObject();
	QPointF endPoint = _connection->getEndPoint(startNode);
	return go.mapToScene(endPoint);
}

QPointF NodeConnectionInteraction::nodePortScenePosition(QPoint portPoint) const
{
	QPointF p = _node->portScenePosition(portPoint);
	NodeGraphicsObject& ngo = _node->nodeGraphicsObject();
	return ngo.sceneTransform().map(p);
}

QPoint NodeConnectionInteraction::nodePortPointUnderScenePoint(
	QPointF const& scenePoint) const
{
	QTransform sceneTransform =
		_node->nodeGraphicsObject().sceneTransform();

	QPoint portPoint;
	if (!_node->checkHitScenePoint(portPoint, scenePoint, sceneTransform))
		portPoint = QPoint(-1, -1);
	return portPoint;
}

bool NodeConnectionInteraction::nodePortIsEmpty(QPoint portPoint) const
{
	auto const& entries = _node->getEntries();

	for (auto& c : entries)
		if (c.first == portPoint)
			return false;
	return true;
}
