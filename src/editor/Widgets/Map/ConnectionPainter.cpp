#include "ConnectionPainter.hpp"

#include <QtGui/QIcon>

#include "ConnectionGraphicsObject.hpp"
#include "Connection.hpp"

#include "MapStyle.hpp"

#define NODE_DEBUG_DRAWING 1

static QPainterPath cubicPath(Connection const& connection)
{
	QPointF const& source = connection.getEnd();
	QPointF const& sink = connection.getStart();

	auto c1c2 = connection.pointsC1C2();

	// cubic spline
	QPainterPath cubic(source);

	cubic.cubicTo(c1c2.first, c1c2.second, sink);

	return cubic;
}

QPainterPath ConnectionPainter::getPainterStroke(Connection const& connection)
{
	auto cubic = cubicPath(connection);

	QPointF const& source = connection.getEnd();
	QPainterPath result(source);

	unsigned segments = 20;

	for (auto i = 0ul; i < segments; ++i) {
		double ratio = double(i + 1) / segments;
		result.lineTo(cubic.pointAtPercent(ratio));
	}

	QPainterPathStroker stroker;
	stroker.setWidth(10.0);

	return stroker.createStroke(result);
}

#ifdef NODE_DEBUG_DRAWING
static void debugDrawing(QPainter* painter, Connection const& connection)
{
	Q_UNUSED(painter);
	Q_UNUSED(connection);

	{
		QPointF const& source = connection.getEnd();
		QPointF const& sink = connection.getStart();

		auto points = connection.pointsC1C2();

		painter->setPen(Qt::red);
		painter->setBrush(Qt::red);

		painter->drawLine(QLineF(source, points.first));
		painter->drawLine(QLineF(points.first, points.second));
		painter->drawLine(QLineF(points.second, sink));
		painter->drawEllipse(points.first, 3, 3);
		painter->drawEllipse(points.second, 3, 3);

		painter->setBrush(Qt::NoBrush);

		painter->drawPath(cubicPath(connection));
	}

	{
		painter->setPen(Qt::yellow);

		painter->drawRect(connection.boundingRect());
	}
}
#endif

static void drawSketchLine(QPainter* painter, Connection const& connection)
{
	if (!connection.complete()) {
		auto const& style = MapStyle::instance();

		QPen p;
		p.setWidth(style.ConnectionConstructionLineWidth);
		p.setColor(style.ConnectionConstructionColor);
		p.setStyle(Qt::DashLine);

		painter->setPen(p);
		painter->setBrush(Qt::NoBrush);

		auto cubic = cubicPath(connection);
		// cubic spline
		painter->drawPath(cubic);
	}
}

static void drawHoveredOrSelected(QPainter* painter,
								  Connection const& connection)
{
	bool const hovered = connection.hovered();

	auto const& graphicsObject = connection.getConnectionGraphicsObject();

	bool const selected = graphicsObject.isSelected();

	// drawn as a fat background
	if (hovered || selected) {
		QPen p;

		auto const& style = MapStyle::instance();

		p.setWidth(2 * style.ConnectionLineWidth);
		p.setColor(selected ? style.ConnectionSelectedHaloColor
							: style.ConnectionHoveredColor);

		painter->setPen(p);
		painter->setBrush(Qt::NoBrush);

		// cubic spline
		auto cubic = cubicPath(connection);
		painter->drawPath(cubic);
	}
}

static void drawNormalLine(QPainter* painter, Connection const& connection)
{
	if (!connection.complete())
		return;

	auto const& style = MapStyle::instance();

	// draw normal line
	QPen p;
	p.setWidth(style.ConnectionLineWidth);

	auto const& graphicsObject = connection.getConnectionGraphicsObject();
	bool const selected = graphicsObject.isSelected();
	auto cubic = cubicPath(connection);

	p.setColor(style.ConnectionNormalColor);
	if (selected)
		p.setColor(style.ConnectionSelectedColor);

	painter->setPen(p);
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(cubic);
}

void ConnectionPainter::paint(QPainter* painter, Connection const& connection)
{
	drawHoveredOrSelected(painter, connection);

	drawSketchLine(painter, connection);

	drawNormalLine(painter, connection);

#ifdef NODE_DEBUG_DRAWING
	debugDrawing(painter, connection);
#endif

	// draw end points
	QPointF const& source = connection.getEnd();
	QPointF const& sink = connection.getStart();

	auto const& style = MapStyle::instance();

	painter->setPen(style.ConnectionConstructionColor);
	painter->setBrush(style.ConnectionConstructionColor);
	double const pointRadius = style.ConnectionPointDiameter / 2.0;
	painter->drawEllipse(source, pointRadius, pointRadius);
	painter->drawEllipse(sink, pointRadius, pointRadius);
}
