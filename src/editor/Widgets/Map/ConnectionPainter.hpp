#pragma once

#include <QtGui/QPainter>

class Connection;

class ConnectionPainter
{
public:
	static void paint(QPainter* painter, Connection const& connection);
	static QPainterPath getPainterStroke(Connection const& connection);
};
