#ifndef MAPVIEW_HPP
#define MAPVIEW_HPP

#include <QGraphicsView>

class MapView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit MapView(QWidget *parent = 0);
	~MapView();

public Q_SLOTS:
	void scaleUp();
	void scaleDown();
	void deleteSelectedNodes();

protected:
//	bool event(QEvent *event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void drawBackground(QPainter* painter, const QRectF& r) override;
	void showEvent(QShowEvent *event) override;

private:
	QPointF m_clickPos;
	QGraphicsScene m_scene;
};

#endif // MAPVIEW_HPP
