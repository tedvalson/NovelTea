#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <functional>
#include <memory>

#include "QUuidStdHash.hpp"

class Node;
class NodeGraphicsObject;
class Connection;
class ConnectionGraphicsObject;

namespace NovelTea {
class Map;
}

/// Scene holds connections and nodes.
class FlowScene : public QGraphicsScene
{
	Q_OBJECT
public:
	FlowScene(QObject* parent = Q_NULLPTR);

	~FlowScene();

public:
	std::shared_ptr<NovelTea::Map> toMapEntity() const;

	std::shared_ptr<Connection> createConnection(Node& node,
												 QPoint portPoint);

	std::shared_ptr<Connection> createConnection(Node& nodeIn,
												 QPoint portPointIn,
												 Node& nodeOut,
												 QPoint portPointOut);

	void deleteConnection(Connection& connection);

	Node& createNode();
	void removeNode(Node& node);

	void iterateOverNodes(std::function<void(Node*)> const& visitor);

	QPointF getNodePosition(Node const& node) const;
	void setNodePosition(Node& node, QPointF const& pos) const;
	QSizeF getNodeSize(Node const& node) const;

public:
	std::unordered_map<QUuid, std::unique_ptr<Node>> const& nodes() const;
	std::unordered_map<QUuid, std::shared_ptr<Connection>> const& connections() const;

//	std::vector<Node*> allNodes() const;
	std::vector<Node*> selectedNodes() const;
	static void snapPointToGrid(QPointF& point);

public:
	void clearScene();
	void printInfo();
	void checkNodeDoorways(Node& n);
	void checkConnectionDoorway(Connection const& c);

Q_SIGNALS:

	/**
	 * @brief Node has been created but not on the scene yet.
	 * @see nodePlaced()
	 */
	void nodeCreated(Node& n);

	/**
	 * @brief Node has been added to the scene.
	 * @details Connect to this signal if need a correct position of node.
	 * @see nodeCreated()
	 */
	void nodePlaced(Node& n);
	void nodeDeleted(Node& n);
	void connectionCreated(Connection const& c);
	void connectionDeleted(Connection const& c);
	void nodeMoved(Node& n, const QPointF& newLocation);
	void nodeResized(Node& n, const QSizeF& newSize);
	void nodeDoubleClicked(Node& n);
	void connectionHovered(Connection& c, QPoint screenPos);
	void nodeHovered(Node& n, QPoint screenPos);
	void connectionHoverLeft(Connection& c);
	void nodeHoverLeft(Node& n);
	void nodeContextMenu(Node& n, const QPointF& pos);

private:
	using SharedConnection = std::shared_ptr<Connection>;
	using UniqueNode = std::unique_ptr<Node>;

	std::unordered_map<QUuid, SharedConnection> _connections;
	std::unordered_map<QUuid, UniqueNode> _nodes;

private Q_SLOTS:

	void setupConnectionSignals(Connection const& c);
};

Node* locateNodeAt(QPointF scenePoint, FlowScene& scene,
				   QTransform const& viewTransform);
