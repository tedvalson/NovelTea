#pragma once

#include <NovelTea/Map.hpp>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"

class Connection;
class NodeGraphicsObject;

class Node : public QObject
{
	Q_OBJECT

public:
	enum ReactToConnectionState { REACTING, NOT_REACTING };

	Node();
	virtual ~Node();

public:
	QUuid id() const;
	void reactToPossibleConnection(QPointF const& scenePoint);
	void resetReactionToConnection();

public:
	NodeGraphicsObject const& nodeGraphicsObject() const;
	NodeGraphicsObject& nodeGraphicsObject();
	void setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

	void recalculateNodeGeometry();

public:
	const QString& name() const { return _name; }
	void setName(const QString& name);
	unsigned int height() const { return _height; }
	void setHeight(unsigned int h);
	unsigned int width() const { return _width; }
	void setWidth(unsigned int w);

	unsigned int minHeight() const { return _minHeight; }
	void setMinHeight(unsigned int h);
	unsigned int minWidth() const { return _minWidth; }
	void setMinWidth(unsigned int w);

	bool hovered() const { return _hovered; }

	void setHovered(unsigned int h) { _hovered = h; }

	QPointF const& draggingPos() const { return _draggingPos; }

	void setDraggingPosition(QPointF const& pos) { _draggingPos = pos; }

	bool onHorizontalWall(const QPoint& p);
	bool onVerticalWall(const QPoint& p);

	QRectF boundingRect() const;

	// TODO removed default QTransform()
	QPointF portScenePosition(QPoint portPoint) const;

	bool checkHitScenePoint(QPoint& result, QPointF point,
							QTransform const& t = QTransform()) const;

	QPointF snapToPortPoint(const QPointF& pos,
							QTransform const& t = QTransform()) const;

	QRect resizeRect() const;

	void setScript(const std::string &script) { _script = script; }
	const std::string &getScript() const { return _script; }

	void setStyle(NovelTea::RoomStyle style) { _style = style; }
	NovelTea::RoomStyle getStyle() const { return _style; }

	void setRoomIds(const std::vector<std::string> &roomIds) { _roomIds = roomIds; }
	const std::vector<std::string> &getRoomIds() const { return _roomIds; }

public:
	static unsigned int borderThickness;
	static unsigned int snapValue;

public:
	using Connections = std::pair<QPoint, Connection*>;
	std::vector<Connections> const& getEntries() const;
	std::vector<Connections>& getEntries();
	Connection* connection(QPoint portPoint) const;
	void setConnection(QPoint portPoint, Connection& connection);
	void eraseConnection(QPoint portPoint);
	void setReaction(ReactToConnectionState reaction,
					 const QPoint& portPoint = QPoint());

	ReactToConnectionState reaction() const;
	const QPoint& reactingPortPoint() const;
	bool isReacting() const;
	void setResizing(bool resizing);
	bool resizing() const;

public Q_SLOTS:
	void onNodeSizeUpdated();

private:
	QUuid _uid;
	std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
	QString _name;

	unsigned int _width;
	unsigned int _height;

	unsigned int _minWidth;
	unsigned int _minHeight;

	bool _hovered;
	QPointF _draggingPos;

	std::vector<Connections> _connections;
	ReactToConnectionState _reaction;
	QPoint _reactingPoint;
	bool _resizing;

	std::string _script;
	std::vector<std::string> _roomIds;
	NovelTea::RoomStyle _style;
};
