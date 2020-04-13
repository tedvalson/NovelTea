#include "NovelTeaWidget.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateIdentifiers.hpp>
#include <QMouseEvent>
#include <iostream>


NovelTeaWidget::NovelTeaWidget(QWidget *parent) :
	SFMLWidget(parent),
	_engine(nullptr),
//	_internalSize(480, 852)
	_internalSize(480, 700)
{
	_internalRatio = _internalSize.x / _internalSize.y;
	_view.reset(sf::FloatRect(0, 0, _internalSize.x, _internalSize.y));

	_bg.setSize(_internalSize);
	_bg.setFillColor(sf::Color::White);
}

NovelTeaWidget::~NovelTeaWidget()
{
//	deleteTexture(texture);
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

void NovelTeaWidget::mousePressEvent(QMouseEvent *e)
{
	sf::Event event;
	auto coords = mapPixelToCoords({e->x(), e->y()});
	event.type = sf::Event::TouchBegan;
	event.touch = sf::Event::TouchEvent {0, coords.x, coords.y};
	_engine->processEvent(event);
}

void NovelTeaWidget::mouseReleaseEvent(QMouseEvent *e)
{
	sf::Event event;
	auto coords = mapPixelToCoords({e->x(), e->y()});
	event.type = sf::Event::TouchEnded;
	event.touch = sf::Event::TouchEvent {0, coords.x, coords.y};
	_engine->processEvent(event);
}

void NovelTeaWidget::mouseMoveEvent(QMouseEvent *e)
{
	sf::Event event;
	auto coords = mapPixelToCoords({e->x(), e->y()});
	event.type = sf::Event::TouchMoved;
	event.touch = sf::Event::TouchEvent {0, coords.x, coords.y};
	_engine->processEvent(event);
}

void NovelTeaWidget::onInit()
{
	NovelTea::EngineConfig config;
	config.width = _internalSize.x;
	config.height = _internalSize.y;
	config.initialState = NovelTea::StateID::Main;
	_engine = new NovelTea::Engine(config);
	_engine->initialize();
}

void NovelTeaWidget::onResize()
{
	sf::FloatRect viewport;
	sf::Vector2f widgetSize(getSize().x, getSize().y);
	auto ratio = widgetSize.x / widgetSize.y;

	if (ratio < _internalRatio)
	{
		viewport.width = 1.f;
		viewport.height = ratio / _internalRatio;
		viewport.left = 0.f;
		viewport.top = (1.f - viewport.height) / 2.f;
	}
	else
	{
		viewport.width = _internalRatio / ratio;
		viewport.height = 1.f;
		viewport.left = (1.f - viewport.width) / 2.f;
		viewport.top = 0.f;
	}

	_view.setViewport(viewport);
	setView(_view);

//	std::cout << "resized " << getSize().x << ", " << getSize().y << std::endl;
}

void NovelTeaWidget::onUpdate(float delta)
{
	// TODO: fix QT / SFML window message conflict
	sf::Event event;
	while (pollEvent(event))
		std::cout << "pollEvent worked!" << std::endl;

	_engine->update(delta);
	draw(_bg);
	_engine->render(*this);

//	std::cout << "onupdate" << std::endl;

	// Change time here according to FPS needs?
//	timer.start(1, this);
}
