#pragma once

#include "Node.hpp"
#include "Connection.hpp"

class FlowScene;

/// Class performs various operations on the Node and Connection pair.
/// An instance should be created on the stack and destroyed when
/// the operation is completed
class NodeConnectionInteraction
{
public:
	NodeConnectionInteraction(Node& node, Connection& connection,
							  FlowScene& scene);

	bool canConnect(QPoint& portPoint) const;
	bool tryConnect() const;

	bool moveConnection(const QPoint& newPoint);

	bool disconnect(bool startNode) const;
	bool disconnect(Node* node) const;

private:
	QPointF connectionEndScenePosition(bool startNode) const;
	QPointF nodePortScenePosition(QPoint portPoint) const;
	QPoint nodePortPointUnderScenePoint(QPointF const& p) const;
	bool nodePortIsEmpty(QPoint portPoint) const;

private:
	Node* _node;
	Connection* _connection;
	FlowScene* _scene;
};
