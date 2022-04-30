#pragma once

#include <QtGui/QPainter>

class Node;
class NodeGraphicsObject;
class FlowScene;

class NodePainter
{
public:
	NodePainter();

public:
	static void paint(QPainter* painter, Node& node,
					  FlowScene const& scene);
	static void drawBorderRect(QPainter* painter, Node const& node,
							   NodeGraphicsObject const& graphicsObject);
	static void drawNodeRect(QPainter* painter, Node const& node,
							 NodeGraphicsObject const& graphicsObject);
	static void drawModelName(QPainter* painter, Node const& node);
	static void drawConnectionPoints(QPainter* painter, Node const& node,
									 FlowScene const& scene);
	static void drawFilledConnectionPoints(QPainter* painter,
										   Node const& node);
	static void drawResizeRect(QPainter* painter, Node const& node);
};
