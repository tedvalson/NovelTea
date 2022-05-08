#include "NodePainter.hpp"

#include <cmath>

#include "MapStyle.hpp"
#include "NodeGraphicsObject.hpp"
#include "Node.hpp"
#include "FlowScene.hpp"

void NodePainter::paint(QPainter* painter, Node& node, FlowScene const& scene)
{
	NodeGraphicsObject const& graphicsObject = node.nodeGraphicsObject();

	drawBorderRect(painter, node, graphicsObject);
	drawNodeRect(painter, node, graphicsObject);
	drawConnectionPoints(painter, node, scene);
	drawFilledConnectionPoints(painter, node);
	drawModelName(painter, node);
	drawResizeRect(painter, node);
}

void NodePainter::drawBorderRect(QPainter* painter, Node const& node,
								 NodeGraphicsObject const& graphicsObject)
{
	auto const& style = MapStyle::instance();

	auto color = graphicsObject.isSelected()
					 ? style.NodeSelectedBoundaryColor
					 : style.NodeNormalBoundaryColor;

	if (node.hovered()) {
		QPen p(color, style.NodeHoveredPenWidth);
		painter->setPen(p);
	} else {
		QPen p(color, style.NodePenWidth);
		painter->setPen(p);
	}

	QRect rect(0, 0, node.width(), node.height());
	QLinearGradient gradient(QPointF(0.0, 0.0),
							 QPointF(2.0, rect.height()));

	gradient.setColorAt(0.0, style.NodeGradientColor0.darker(110));
	gradient.setColorAt(0.03, style.NodeGradientColor1.darker(110));
	gradient.setColorAt(0.97, style.NodeGradientColor2.darker(110));
	gradient.setColorAt(1.0, style.NodeGradientColor3.darker(110));

	painter->setBrush(gradient);
	painter->drawRect(rect);
}

void NodePainter::drawNodeRect(QPainter* painter, Node const& node,
							   NodeGraphicsObject const& graphicsObject)
{
	auto const& style = MapStyle::instance();

	auto color = graphicsObject.isSelected()
					 ? style.NodeSelectedBoundaryColor
					 : style.NodeNormalBoundaryColor;

	if (node.hovered()) {
		QPen p(color, style.NodeHoveredPenWidth);
		painter->setPen(p);
	} else {
		QPen p(color, style.NodePenWidth);
		painter->setPen(p);
	}

	auto borderThickness = Node::borderThickness;
	QRectF rect(borderThickness, borderThickness,
				node.width() - borderThickness * 2,
				node.height() - borderThickness * 2);

	QLinearGradient gradient(QPointF(0.0, 0.0),
							 QPointF(2.0, rect.height()));

	gradient.setColorAt(0.0, style.NodeGradientColor0);
	gradient.setColorAt(0.03, style.NodeGradientColor1);
	gradient.setColorAt(0.97, style.NodeGradientColor2);
	gradient.setColorAt(1.0, style.NodeGradientColor3);
	painter->setBrush(gradient);
	painter->drawRect(rect);
}

void NodePainter::drawConnectionPoints(QPainter* painter,
									   Node const& node,
									   FlowScene const& scene)
{
	auto const& style = MapStyle::instance();

	float diameter = style.NodeConnectionPointDiameter;
	float r = 5.f;

	bool canConnect = false;

	if (node.isReacting() && canConnect) {
		QPointF p = node.portScenePosition(node.reactingPortPoint());
//		qDebug() << "reacting x:" << p.x() << " y:" << p.y();

		auto diff = node.draggingPos() - p;
		double dist = std::sqrt(QPointF::dotProduct(diff, diff));

		painter->setBrush(style.NodeConnectionPointColor);
		painter->setBrush(Qt::red);

		painter->drawEllipse(p, r, r);
	}
}

void NodePainter::drawFilledConnectionPoints(QPainter* painter,
											 Node const& node)
{
	auto const& style = MapStyle::instance();
	auto diameter = style.NodeConnectionPointDiameter;
	auto& entries = node.getEntries();

	for (auto& c : entries) {
		QPointF p = node.portScenePosition(c.first) - node.nodeGraphicsObject().pos();
		painter->setPen(style.ConnectionNormalColor);
		painter->setBrush(style.ConnectionNormalColor);
		painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
	}
}

void NodePainter::drawModelName(QPainter* painter, Node const& node)
{
	auto const& style = MapStyle::instance();
	auto name = node.name();

	QFont f = painter->font();
	if (name.isEmpty()) {
		name = "[empty]";
		painter->setPen(style.NodeFontColorFaded);
	} else {
		f.setBold(true);
		painter->setPen(style.NodeFontColor);
	}
	painter->setFont(f);

	auto borderThickness = Node::borderThickness;
	QRect rect(borderThickness, borderThickness,
			   node.width() - borderThickness * 2,
			   node.height() - borderThickness * 2);
	painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, name);

	f.setBold(false);
	painter->setFont(f);
}

void NodePainter::drawResizeRect(QPainter* painter, Node const& node)
{
	painter->setBrush(Qt::gray);
	painter->drawEllipse(node.resizeRect());
}
