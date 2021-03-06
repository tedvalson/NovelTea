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

	void setFPS(float fps);
	float getFPS() const;

protected:
	virtual void onInit() = 0;
	virtual void onUpdate(float delta) = 0;
	void onResize() override;
	void showEvent(QShowEvent*) override;
	void hideEvent(QHideEvent*) override;
	void paintEvent(QPaintEvent*) override;
	void resizeEvent(QResizeEvent*);

	void takeWindow();

private:
	QPaintEngine *paintEngine() const override;

	QTimer _timer;
	bool _initialized;
	int _lastTime;
	float m_fps;
};

#endif // SFMLWIDGET_H
