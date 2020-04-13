#ifndef NOVELTEAWIDGET_H
#define NOVELTEAWIDGET_H

#include "SFMLWidget.h"
#include <json.hpp>

using json = nlohmann::json;


namespace NovelTea {
class Engine;
//class InputManager;
}


class NovelTeaWidget : public SFMLWidget
{
public:
	explicit NovelTeaWidget(QWidget *parent = 0);
	~NovelTeaWidget();

	json processData(json jsonData);

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
//	void timerEvent(QTimerEvent *e);

//	void initializeGL();
//	void resizeGL(int w, int h);
//	void paintGL();

	virtual void onInit();
	virtual void onResize();
	virtual void onUpdate(float delta);

private:
	NovelTea::Engine *_engine;
	sf::View _view;
	sf::RectangleShape _bg;
//	NovelTea::InputManager *_inputManager;

//	QBasicTimer timer;
	sf::Vector2f _internalSize;
	float _internalRatio;
};

#endif // NOVELTEAWIDGET_H
