#include "NovelTeaWidget.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QMouseEvent>
#include <iostream>


NovelTeaWidget::NovelTeaWidget(QWidget *parent) :
	SFMLWidget(parent),
	_engine(nullptr),
//	_internalSize(480, 852)
	_internalSize(480, 700)
{
	_internalRatio = _internalSize.x / _internalSize.y;

	NovelTea::EngineConfig config;
	config.width = _internalSize.x;
	config.height = _internalSize.y;
	config.initialState = NovelTea::StateID::Editor;
	_engine = new NovelTea::Engine(config);
	_engine->initialize();
	_engine->update(0.f); // This triggers update of state stack
}

NovelTeaWidget::~NovelTeaWidget()
{
	if (_engine)
		delete _engine;
}

json NovelTeaWidget::processData(json jsonData)
{
	json resp;
	auto ptr = static_cast<json*>(_engine->processData(&jsonData));
	if (ptr)
	{
		resp = *ptr;
		delete ptr;
	}
	return resp;
}

void NovelTeaWidget::setMode(NovelTea::StateEditorMode mode)
{
	json jdata;
	jdata["event"] = "mode";
	jdata["mode"] = mode;
	processData(jdata);
}

void NovelTeaWidget::mousePressEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseButtonPressed;
	event.mouseButton = sf::Event::MouseButtonEvent {sf::Mouse::Button::Left, e->x(), e->y()};
	_engine->processEvent(event);
}

void NovelTeaWidget::mouseReleaseEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseButtonReleased;
	event.mouseButton = sf::Event::MouseButtonEvent {sf::Mouse::Button::Left, e->x(), e->y()};
	_engine->processEvent(event);
}

void NovelTeaWidget::mouseMoveEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseMoved;
	event.mouseButton = sf::Event::MouseButtonEvent {sf::Mouse::Button::Left, e->x(), e->y()};
	_engine->processEvent(event);
}

void NovelTeaWidget::onInit()
{
}

void NovelTeaWidget::onResize()
{
	_engine->resize(getSize().x, getSize().y);
}

void NovelTeaWidget::onUpdate(float delta)
{
	// TODO: fix QT / SFML window message conflict
	sf::Event event;
	while (pollEvent(event))
		std::cout << "pollEvent worked!" << std::endl;

	_engine->update(delta);
	_engine->render(*this);

//	std::cout << "onupdate" << std::endl;

	// Change time here according to FPS needs?
//	timer.start(1, this);
}
