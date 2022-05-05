#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVariant>
#include <QPointF>
#include <memory>

#include "QUuidStdHash.hpp"

class QPointF;

class Node;
class ConnectionGraphicsObject;

class Connection : public QObject
{
	Q_OBJECT

public:
	Connection(Node& node, QPoint portPoint);

	Connection(Node& nodeStart, QPoint portPointStart, Node& nodeEnd,
			   QPoint portPointEnd);

	Connection(const Connection&) = delete;
	Connection operator=(const Connection&) = delete;

	~Connection();

public:
	QUuid id() const;

	void setGraphicsObject(std::unique_ptr<ConnectionGraphicsObject>&& graphics);
	void setNodeToPort(Node& node, bool startNode, QPoint portPoint);
	void removeFromNodes() const;

public:
	ConnectionGraphicsObject& getConnectionGraphicsObject() const;

	Node* getNode(bool startNode) const;
	Node*& getNode(bool startNode);

	bool startNodeEmpty() const;
	bool endNodeEmpty() const;

	QPoint getPortPoint(bool startNode) const;

	void clearNode(bool startNode);
	bool complete() const;

public:
	QPointF const& getEndPoint(bool startNode) const;
	void setEndPoint(bool startNode, QPointF const& point);
	void moveEndPoint(bool startNode, QPointF const& offset);
	QRectF boundingRect() const;

	std::pair<QPointF, QPointF> pointsC1C2() const;

	QPointF getEnd() const { return _end; }
	QPointF getStart() const { return _start; }

	double lineWidth() const { return _lineWidth; }

	bool hovered() const { return _hovered; }
	void setHovered(bool hovered) { _hovered = hovered; }

	void setScript(const std::string &script) { _script = script; }
	const std::string &getScript() const { return _script; }

public:
	void interactWithNode(Node* node);
	void setLastHoveredNode(Node* node);
	Node* lastHoveredNode() const { return _lastHoveredNode; }
	void resetLastHoveredNode();

Q_SIGNALS:
	void connectionCompleted(Connection const&) const;
	void connectionMadeIncomplete(Connection const&) const;
	void updated(Connection& conn) const;

private:
	QUuid _uid;
	Node* _endNode = nullptr;
	Node* _startNode = nullptr;

	QPoint _endPortPoint;
	QPoint _startPortPoint;
	std::unique_ptr<ConnectionGraphicsObject> _connectionGraphicsObject;

	// local object coordinates
	QPointF _start;
	QPointF _end;
	double _lineWidth;
	bool _hovered;
	Node* _lastHoveredNode;

	std::string _script;
	int _type;
};
