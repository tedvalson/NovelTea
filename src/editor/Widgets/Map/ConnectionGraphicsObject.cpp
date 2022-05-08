#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include "FlowScene.hpp"

#include "Connection.hpp"
#include "ConnectionPainter.hpp"

#include "NodeGraphicsObject.hpp"
#include "NodeConnectionInteraction.hpp"
#include "Node.hpp"

ConnectionGraphicsObject::ConnectionGraphicsObject(FlowScene& scene,
												   Connection& connection)
: _scene(scene)
, _connection(connection)
{
	_scene.addItem(this);

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

	setAcceptHoverEvents(true);

// addGraphicsEffect();

	setZValue(-1);
}

ConnectionGraphicsObject::~ConnectionGraphicsObject()
{
	_scene.removeItem(this);
}

Connection& ConnectionGraphicsObject::connection() { return _connection; }

QRectF ConnectionGraphicsObject::boundingRect() const
{
	return _connection.boundingRect();
}

QPainterPath ConnectionGraphicsObject::shape() const
{
#ifdef DEBUG_DRAWING

// QPainterPath path;

// path.addRect(boundingRect());
// return path;

#else
	return ConnectionPainter::getPainterStroke(_connection);

#endif
}

void ConnectionGraphicsObject::setGeometryChanged() { prepareGeometryChange(); }

void ConnectionGraphicsObject::move()
{
	for (bool startNode : {true, false}) {
		if (auto node = _connection.getNode(startNode)) {
			auto const& nodeGraphics = node->nodeGraphicsObject();

			QPointF scenePos = node->portScenePosition(
				_connection.getPortPoint(startNode));

			QTransform sceneTransform = this->sceneTransform();

			QPointF connectionPos =
				sceneTransform.inverted().map(scenePos);

			_connection.setEndPoint(
				startNode, connectionPos);

			_connection.getConnectionGraphicsObject()
				.setGeometryChanged();
			_connection.getConnectionGraphicsObject().update();
		}
	}
}

void ConnectionGraphicsObject::lock(bool locked)
{
	setFlag(QGraphicsItem::ItemIsMovable, !locked);
	setFlag(QGraphicsItem::ItemIsFocusable, !locked);
	setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}

void ConnectionGraphicsObject::paint(QPainter* painter,
									 QStyleOptionGraphicsItem const* option,
									 QWidget*)
{
	painter->setClipRect(option->exposedRect);

	ConnectionPainter::paint(painter, _connection);
}

void ConnectionGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mousePressEvent(event);
	// event->ignore();
	setZValue(2.0);
}

void ConnectionGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	prepareGeometryChange();

	auto view = static_cast<QGraphicsView*>(event->widget());
	auto node = locateNodeAt(event->scenePos(), _scene, view->transform());

	_connection.interactWithNode(node);
	if (node) {
		node->reactToPossibleConnection(event->scenePos());
	}

	//-------------------

	if (!_connection.complete()) {
		auto t = _scene.views()[0]->transform();
		auto node = locateNodeAt(event->scenePos(), _scene, t);
		QPointF pos = event->pos();
		_scene.snapPointToGrid(pos);
		if (node) {
			auto& obj = node->nodeGraphicsObject();
			pos = node->snapToPortPoint(
				event->scenePos(),
				node->nodeGraphicsObject().sceneTransform());
			pos = mapFromScene(obj.scenePos() + pos);
		}
		_connection.setEndPoint(
			_connection.startNodeEmpty(), pos);
		setZValue(2.0);
	}

	update();
	event->accept();
}

void ConnectionGraphicsObject::mouseReleaseEvent(
	QGraphicsSceneMouseEvent* event)
{
	ungrabMouse();
	event->accept();
	setZValue(-1.0);

	auto node = locateNodeAt(event->scenePos(), _scene,
							 _scene.views()[0]->transform());

	NodeConnectionInteraction interaction(*node, _connection, _scene);

	if (node && interaction.tryConnect()) {
		node->resetReactionToConnection();
	}
	if (!_connection.complete()) {
		_scene.deleteConnection(_connection);
	}
}

void ConnectionGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	_connection.setHovered(true);

	update();
	_scene.connectionHovered(connection(), event->screenPos());
	event->accept();
}

void ConnectionGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	_connection.setHovered(false);

	update();
	_scene.connectionHoverLeft(connection());
	event->accept();
}

void ConnectionGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	_scene.connectionContextMenu(connection(), mapToScene(event->pos()));
}

void ConnectionGraphicsObject::addGraphicsEffect()
{
	auto effect = new QGraphicsBlurEffect;

	effect->setBlurRadius(5);
	setGraphicsEffect(effect);

	// auto effect = new QGraphicsDropShadowEffect;
	// auto effect = new ConnectionBlurEffect(this);
	// effect->setOffset(4, 4);
	// effect->setColor(QColor(Qt::gray).darker(800));
}
