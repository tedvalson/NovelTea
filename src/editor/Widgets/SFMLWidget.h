#ifndef SFMLWIDGET_H
#define SFMLWIDGET_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QLabel>
#include <QTimer>

class SFMLWidget : public QWidget, public sf::RenderWindow
{
//	Q_OBJECT
public:
	explicit SFMLWidget(QWidget *parent = 0);
	virtual ~SFMLWidget();

	static QByteArray getData(QString filepath);
	static int getTimeMs();

protected:
	virtual void onInit() = 0;
	virtual void onUpdate(float delta) = 0;
	virtual void onResize();
	virtual void showEvent(QShowEvent*);
	virtual void hideEvent(QHideEvent*);
	void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private:
	virtual QPaintEngine *paintEngine() const;

	QTimer _timer;
	bool _initialized;
	int _lastTime;
};

#endif // SFMLWIDGET_H
