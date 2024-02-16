#include "FlowScene.hpp"

#include <NovelTea/Map.hpp>
#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtCore/QDebug>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Connection.hpp"
#include "FlowView.hpp"

FlowScene::FlowScene(QObject* parent)
: QGraphicsScene(parent)
{
	setItemIndexMethod(QGraphicsScene::NoIndex);

	connect(this, &FlowScene::connectionCreated, this, &FlowScene::setupConnectionSignals);
	connect(this, &FlowScene::nodeMoved, this, &FlowScene::checkNodeDoorways);
	connect(this, &FlowScene::connectionCreated, this, &FlowScene::checkConnectionDoorway);
	connect(this, &FlowScene::connectionDeleted, this, &FlowScene::checkConnectionDoorway);
}

FlowScene::~FlowScene()
{
	clearScene();
}

std::shared_ptr<NovelTea::Map> FlowScene::toMapEntity(NovelTea::Context *context) const
{
	std::unordered_map<QUuid, int> n;
	auto map = std::make_shared<NovelTea::Map>(context);
	QPointF diff(10000, 10000);
	for (auto &nodePair : _nodes)
	{
		Node* node = nodePair.second.get();
		auto pos = node->nodeGraphicsObject().pos();
		if (pos.x() < diff.x())
			diff.setX(pos.x());
		if (pos.y() < diff.y())
			diff.setY(pos.y());
	}
	for (auto &nodePair : _nodes)
	{
		Node* node = nodePair.second.get();
		auto name = node->name().toStdString();
		auto pos = node->nodeGraphicsObject().pos() - diff;
		pos /= Node::snapValue;
		auto rect = NovelTea::IntRect(pos.x(), pos.y(),
					node->width() / Node::snapValue,
					node->height() / Node::snapValue);
		n[node->id()] = map->addRoom(name, rect, node->getRoomIds(), node->getScript(), node->getStyle());
	}
	for (auto &connPair : _connections)
	{
		Connection* c = connPair.second.get();
		if (!c->complete())
			continue;
		int startNodeIndex = n[c->getNode(true)->id()];
		int endNodeIndex = n[c->getNode(false)->id()];
		QPoint pStart = c->getPortPoint(true);
		QPoint pEnd = c->getPortPoint(false);
		NovelTea::Vector2i startPort(pStart.x(), pStart.y());
		NovelTea::Vector2i endPort(pEnd.x(), pEnd.y());
		map->addConnection(startNodeIndex, endNodeIndex, startPort, endPort, c->getScript(), c->getStyle());
	}
	return map;
}

//------------------------------------------------------------------------------

std::shared_ptr<Connection> FlowScene::createConnection(Node& node,
														QPoint portPoint)
{
	auto connection = std::make_shared<Connection>(node, portPoint);
	auto cgo = std::unique_ptr<ConnectionGraphicsObject>(new ConnectionGraphicsObject(*this, *connection));

	// after this function connection points are set to node port
	connection->setGraphicsObject(std::move(cgo));

	_connections[connection->id()] = connection;

	// Note: this connection isn't truly created yet. It's only partially
	// created.
	// Thus, don't send the connectionCreated(...) signal.

	connect(connection.get(), &Connection::connectionCompleted, this,
			[this](Connection const& c) {
		connectionCreated(c);
	});

	return connection;
}

std::shared_ptr<Connection> FlowScene::createConnection(Node& nodeIn,
														QPoint portPointIn,
														Node& nodeOut,
														QPoint portPointOut)
{
	auto connection = std::make_shared<Connection>(nodeIn, portPointIn,
												   nodeOut, portPointOut);

	auto cgo =
		std::unique_ptr<ConnectionGraphicsObject>(new ConnectionGraphicsObject(*this, *connection));

	nodeIn.setConnection(portPointIn, *connection);
	nodeOut.setConnection(portPointOut, *connection);

	// after this function connection points are set to node port
	connection->setGraphicsObject(std::move(cgo));
	_connections[connection->id()] = connection;
	connectionCreated(*connection);

	return connection;
}

void FlowScene::deleteConnection(Connection& connection)
{
	auto it = _connections.find(connection.id());
	if (it != _connections.end()) {
		connection.removeFromNodes();
		_connections.erase(it);
	}
}

Node& FlowScene::createNode()
{
	auto node = std::unique_ptr<Node>(new Node);
	auto ngo = std::unique_ptr<NodeGraphicsObject>(new NodeGraphicsObject(*this, *node));

	node->setGraphicsObject(std::move(ngo));

	auto nodePtr = node.get();
	_nodes[node->id()] = std::move(node);

	nodeCreated(*nodePtr);
	return *nodePtr;
}

void FlowScene::removeNode(Node& node)
{
	// call signal
	nodeDeleted(node);

	auto const nodeEntries = node.getEntries();

	for (auto& pair : nodeEntries) {
		deleteConnection(*pair.second);
	}

	_nodes.erase(node.id());
}

void FlowScene::iterateOverNodes(std::function<void(Node*)> const& visitor)
{
	for (const auto& _node : _nodes) {
		visitor(_node.second.get());
	}
}

QPointF FlowScene::getNodePosition(const Node& node) const
{
	return node.nodeGraphicsObject().pos();
}

void FlowScene::setNodePosition(Node& node, const QPointF& pos) const
{
	node.nodeGraphicsObject().setPos(pos);
	node.nodeGraphicsObject().moveConnections();
}

QSizeF FlowScene::getNodeSize(const Node& node) const
{
	return QSizeF(node.width(),
				  node.height());
}

std::unordered_map<QUuid, std::unique_ptr<Node>> const& FlowScene::nodes() const
{
	return _nodes;
}

std::unordered_map<QUuid, std::shared_ptr<Connection>> const&
FlowScene::connections() const
{
	return _connections;
}

std::vector<Node*> FlowScene::selectedNodes() const
{
	QList<QGraphicsItem*> graphicsItems = selectedItems();

	std::vector<Node*> ret;
	ret.reserve(graphicsItems.size());

	for (QGraphicsItem* item : graphicsItems) {
		auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

		if (ngo != nullptr) {
			ret.push_back(&ngo->node());
		}
	}

	return ret;
}

void FlowScene::snapPointToGrid(QPointF& point)
{
	const float snapWidth = Node::snapValue;
	auto x = round(point.x() / snapWidth) * snapWidth;
	auto y = round(point.y() / snapWidth) * snapWidth;
	point = QPointF(x, y);
}

//------------------------------------------------------------------------------

void FlowScene::clearScene()
{
	// TODO: This may not be necessary without data propagation now
	while (_connections.size() > 0) {
		deleteConnection(*_connections.begin()->second);
	}

	while (_nodes.size() > 0) {
		removeNode(*_nodes.begin()->second);
	}
}

void FlowScene::printInfo()
{
	for (auto& conn : connections()) {
		auto c = conn.second;
		auto n1 = c->getNode(true);
		auto n2 = c->getNode(false);
		auto p1 = c->getPortPoint(true);
		auto p2 = c->getPortPoint(false);
		qDebug() << "connection: " << p1.x() << "," << p1.y() << "  "
				  << (n1 ? n1->id().toString() : "<empty>");
		qDebug() << "    " << p2.x() << "," << p2.y() << "  "
				  << (n2 ? n2->id().toString() : "<empty>");
	}
}

void FlowScene::checkNodeDoorways(Node &n)
{
	auto& connections = n.getEntries();
	for (auto& c : connections)
		c.second->checkDoorway();
}

void FlowScene::checkConnectionDoorway(const Connection &c)
{
	c.checkDoorway();
}

void FlowScene::setupConnectionSignals(Connection const& c)
{
	connect(&c, &Connection::connectionMadeIncomplete, this,
			&FlowScene::connectionDeleted, Qt::UniqueConnection);
}

//------------------------------------------------------------------------------
Node* locateNodeAt(QPointF scenePoint, FlowScene& scene,
				   QTransform const& viewTransform)
{
	// items under cursor
	QList<QGraphicsItem*> items =
		scene.items(scenePoint, Qt::IntersectsItemShape,
					Qt::DescendingOrder, viewTransform);

	//// items convertable to NodeGraphicsObject
	std::vector<QGraphicsItem*> filteredItems;

	std::copy_if(items.begin(), items.end(),
				 std::back_inserter(filteredItems),
				 [](QGraphicsItem* item) {
		return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
	});

	Node* resultNode = nullptr;

	if (!filteredItems.empty()) {
		QGraphicsItem* graphicsItem = filteredItems.front();
		auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

		resultNode = &ngo->node();
	}

	return resultNode;
}
