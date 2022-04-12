#include "MapView.hpp"
#include <QWheelEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <iostream>

MapView::MapView(QWidget *parent)
: QGraphicsView(parent)
{
	setDragMode(QGraphicsView::ScrollHandDrag);
	setRenderHint(QPainter::Antialiasing);
	setBackgroundBrush(Qt::white);

	//setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	//setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setCacheMode(QGraphicsView::CacheBackground);

	auto p = new QGraphicsProxyWidget();
	auto w = new QLineEdit("Test");
	p->setWidget(w);
	m_scene.addRect(0, 0, 100, 100);
	m_scene.addText("Test");
	m_scene.addItem(p);
	setScene(&m_scene);
}

MapView::~MapView()
{

}

void MapView::scaleUp()
{
	double const step   = 1.2;
	double const factor = std::pow(step, 1.0);
	QTransform t = transform();

	if (t.m11() > 2.0)
		return;

	scale(factor, factor);
}

void MapView::scaleDown()
{
	double const step   = 1.2;
	double const factor = std::pow(step, -1.0);
	scale(factor, factor);
}

void MapView::deleteSelectedNodes()
{

}

void MapView::contextMenuEvent(QContextMenuEvent *event)
{
	if (itemAt(event->pos()))
	{
		QGraphicsView::contextMenuEvent(event);
		return;
	}
}

void MapView::wheelEvent(QWheelEvent *event)
{
	QPoint delta = event->angleDelta();

	if (delta.y() == 0)
	{
		event->ignore();
		return;
	}

	double const d = delta.y() / std::abs(delta.y());

	if (d > 0.0)
		scaleUp();
	else
		scaleDown();
}

void MapView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Shift:
			setDragMode(QGraphicsView::RubberBandDrag);
			break;
		default:
			break;
	}
	QGraphicsView::keyPressEvent(event);
}

void MapView::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Shift:
			setDragMode(QGraphicsView::ScrollHandDrag);
			break;
		default:
			break;
	}
	QGraphicsView::keyReleaseEvent(event);
}

void MapView::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
	if (event->button() == Qt::LeftButton)
	{
		m_clickPos = mapToScene(event->pos());
	}
}

void MapView::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);
	if (scene()->mouseGrabberItem() == nullptr && event->buttons() == Qt::LeftButton)
	{
		// Make sure shift is not being pressed
		if ((event->modifiers() & Qt::ShiftModifier) == 0)
		{
			QPointF difference = m_clickPos - mapToScene(event->pos());
			setSceneRect(sceneRect().translated(difference.x(), difference.y()));
		}
	}
}

void MapView::drawBackground(QPainter *painter, const QRectF &r)
{
	QGraphicsView::drawBackground(painter, r);

	auto drawGrid = [&](double gridStep)
	{
		QRect   windowRect = rect();
		QPointF tl = mapToScene(windowRect.topLeft());
		QPointF br = mapToScene(windowRect.bottomRight());

		double left   = std::floor(tl.x() / gridStep - 0.5);
		double right  = std::floor(br.x() / gridStep + 1.0);
		double bottom = std::floor(tl.y() / gridStep - 0.5);
		double top    = std::floor (br.y() / gridStep + 1.0);

		// vertical lines
		for (int xi = int(left); xi <= int(right); ++xi)
		{
			QLineF line(xi * gridStep, bottom * gridStep,
			xi * gridStep, top * gridStep );

			painter->drawLine(line);
		}

		// horizontal lines
		for (int yi = int(bottom); yi <= int(top); ++yi)
		{
			QLineF line(left * gridStep, yi * gridStep,
			right * gridStep, yi * gridStep );
			painter->drawLine(line);
		}
	};

	QBrush bBrush = backgroundBrush();
	QPen pfine(Qt::lightGray, 1.0);

	painter->setPen(pfine);
	drawGrid(15);

	QPen p(Qt::gray, 1.0);

	painter->setPen(p);
	drawGrid(150);
}

void MapView::showEvent(QShowEvent *event)
{
	m_scene.setSceneRect(this->rect());
	QGraphicsView::showEvent(event);
}
