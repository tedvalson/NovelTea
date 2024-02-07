#ifndef NOVELTEAWIDGET_H
#define NOVELTEAWIDGET_H

#include "SFMLWidget.h"
#include <NovelTea/Game.hpp>
#include <NovelTea/json.hpp>

using json = sj::JSON;


namespace NovelTea {
class EngineSFML;
class EventManager;
enum class StateEditorMode;
}


class NovelTeaWidget : public SFMLWidget
{
public:
	explicit NovelTeaWidget(QWidget *parent = 0);
	virtual ~NovelTeaWidget();

	void setMode(NovelTea::StateEditorMode mode);
	void setTestMode(void *ptrCallback);

	std::shared_ptr<NovelTea::EventManager> events();

	void reset();

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
	NovelTea::Context *m_context;
	NovelTea::EngineSFML *m_engine;
//	NovelTea::InputManager *_inputManager;

//	QBasicTimer timer;
	sf::Vector2f m_internalSize;
	float m_internalRatio;
	bool m_useStateEventManager;
};

#endif // NOVELTEAWIDGET_H
