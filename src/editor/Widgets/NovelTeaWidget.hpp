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
	virtual ~NovelTeaWidget();

	json processData(json jsonData);

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
//	void timerEvent(QTimerEvent *e);

//	void initializeGL();
//	void resizeGL(int w, int h);
//	void paintGL();

	void onInit() override;
	void onResize() override;
	void onUpdate(float delta) override;

private:
	NovelTea::Engine *_engine;
//	NovelTea::InputManager *_inputManager;

//	QBasicTimer timer;
	sf::Vector2f _internalSize;
	float _internalRatio;
};

#endif // NOVELTEAWIDGET_H
