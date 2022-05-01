#include "NodeGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "Node.hpp"
#include "NodeConnectionInteraction.hpp"

#include "MapStyle.hpp"

NodeGraphicsObject::NodeGraphicsObject(FlowScene& scene, Node& node)
: _scene(scene)
, _node(node)
, _locked(false)
{
	_scene.addItem(this);

	setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

	setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	auto const& style = MapStyle::instance();

	auto effect = new QGraphicsDropShadowEffect;
	effect->setOffset(4, 4);
	effect->setBlurRadius(20);
	effect->setColor(style.NodeShadowColor);

	setGraphicsEffect(effect);
	setOpacity(style.NodeOpacity);
	setAcceptHoverEvents(true);
	setZValue(0);

	// connect to the move signals to emit the move signals in FlowScene
	auto onMoveSlot = [this] { _scene.nodeMoved(_node, pos()); };
	connect(this, &QGraphicsObject::xChanged, this, onMoveSlot);
	connect(this, &QGraphicsObject::yChanged, this, onMoveSlot);
}

NodeGraphicsObject::~NodeGraphicsObject() { _scene.removeItem(this); }

Node& NodeGraphicsObject::node() { return _node; }

Node const& NodeGraphicsObject::node() const { return _node; }

QRectF NodeGraphicsObject::boundingRect() const { return _node.boundingRect(); }

void NodeGraphicsObject::setGeometryChanged() { prepareGeometryChange(); }

void NodeGraphicsObject::moveConnections() const
{
	auto const& connectionEntries = _node.getEntries();

	for (auto const& con : connectionEntries)
		con.second->getConnectionGraphicsObject().move();
}

void NodeGraphicsObject::lock(bool locked)
{
	_locked = locked;

	setFlag(QGraphicsItem::ItemIsMovable, !locked);
	setFlag(QGraphicsItem::ItemIsFocusable, !locked);
	setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}

void NodeGraphicsObject::paint(QPainter* painter,
							   QStyleOptionGraphicsItem const* option, QWidget*)
{
	painter->setClipRect(option->exposedRect);
	NodePainter::paint(painter, _node, _scene);
}

QVariant NodeGraphicsObject::itemChange(GraphicsItemChange change,
										const QVariant& value)
{
	if (change == ItemPositionChange && scene()) {
		QPointF pos = value.toPointF();
		_scene.snapPointToGrid(pos);
		moveConnections();
		return pos;
	}

	return QGraphicsItem::itemChange(change, value);
}

void NodeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (_locked)
		return;

	// deselect all other items after this one is selected
	if (!isSelected() && !(event->modifiers() & Qt::ControlModifier)) {
		_scene.clearSelection();
	}

	// TODO do not pass sceneTransform
	QPoint portPoint;
	if (_node.checkHitScenePoint(portPoint, event->scenePos(),
								 sceneTransform())) {
		auto c = _node.connection(portPoint);

		if (c) {
			NodeConnectionInteraction interaction(_node, *c,
												  _scene);
			interaction.disconnect(&_node);
			c->getConnectionGraphicsObject().setZValue(2.0);
			update();
		} else {  // initialize new Connection
			// todo add to FlowScene
			auto connection =
				_scene.createConnection(_node, portPoint);

			_node.setConnection(portPoint, *connection);
			_node.recalculateNodeGeometry();
			connection->getConnectionGraphicsObject().grabMouse();
		}
	}

	auto pos = event->pos();
	if (_node.resizeRect().contains(QPoint(pos.x(), pos.y()))) {
		_node.setResizing(true);
		_sizeBeforeResize = QSize(_node.width(), _node.height());
		_resizeStartPos = event->pos();
	}
}

void NodeGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (_node.resizing()) {
		auto diff = event->pos() - _resizeStartPos;
		prepareGeometryChange();
		auto h = _node.height();
		auto w = _node.width();
		auto x = w / Node::snapValue - 1;
		auto y = h / Node::snapValue - 1;
		auto newH = std::max(static_cast<unsigned int>(
								 _sizeBeforeResize.height() + diff.y()),
							 _node.minHeight());
		auto newW = std::max(static_cast<unsigned int>(
								 _sizeBeforeResize.width() + diff.x()),
							 _node.minWidth());
		auto newX = newW / Node::snapValue - 1;
		auto newY = newH / Node::snapValue - 1;
		if (h != newH || w != newW) {
			auto const connectionEntries = _node.getEntries();
			_node.setHeight(newH);
			_node.setWidth(newW);
			for (auto& con : connectionEntries) {
				Connection* c = con.second;
				QPoint p = con.first;

				if (h != newH && p.y() == y)
					p.setY(newY);
				if (w != newW && p.x() == x)
					p.setX(newX);

				NodeConnectionInteraction interaction(_node, *c,
													  _scene);
				interaction.moveConnection(p);
			}
			_scene.nodeResized(_node, QSizeF(newW, newH));
		}
		update();
		moveConnections();
		event->accept();
	} else {
		QGraphicsObject::mouseMoveEvent(event);

		if (event->lastPos() != event->pos())
			moveConnections();

		event->ignore();
	}

	QRectF r = scene()->sceneRect();

	r = r.united(mapToScene(boundingRect()).boundingRect());

	scene()->setSceneRect(r);
}

void NodeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	_node.setResizing(false);

	QGraphicsObject::mouseReleaseEvent(event);

	// position connections precisely after fast node move
	moveConnections();
}

void NodeGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	// bring all the colliding nodes to background
	QList<QGraphicsItem*> overlapItems = collidingItems();

	for (QGraphicsItem* item : overlapItems) {
		if (item->zValue() > 0.0) {
			//	  item->setZValue(0.0);
		}
	}

	// bring this node forward
	setZValue(1.0);

	_node.setHovered(true);
	update();
	_scene.nodeHovered(node(), event->screenPos());
	event->accept();
}

void NodeGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	_node.setHovered(false);
	update();
	_scene.nodeHoverLeft(node());
	event->accept();
}

void NodeGraphicsObject::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
	auto pos = event->pos();

	if (_node.resizeRect().contains(QPoint(pos.x(), pos.y()))) {
		setCursor(QCursor(Qt::SizeFDiagCursor));
	} else {
		setCursor(QCursor());
	}

	event->accept();
}

void NodeGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	_scene.printInfo();
	QGraphicsItem::mouseDoubleClickEvent(event);

	_scene.nodeDoubleClicked(node());
}

void NodeGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	_scene.nodeContextMenu(node(), mapToScene(event->pos()));
}
