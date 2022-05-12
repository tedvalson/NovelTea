#include "NovelTeaWidget.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QMouseEvent>
#include <iostream>


NovelTeaWidget::NovelTeaWidget(QWidget *parent) :
	SFMLWidget(parent),
	m_engine(nullptr),
//	_internalSize(480, 852)
	m_internalSize(480, 700)
{
	m_internalRatio = m_internalSize.x / m_internalSize.y;
	reset();
}

NovelTeaWidget::~NovelTeaWidget()
{
	if (m_engine)
		delete m_engine;
}

json NovelTeaWidget::processData(json jsonData)
{
	json resp;
	auto ptr = static_cast<json*>(m_engine->processData(&jsonData));
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
	jdata["mode"] = static_cast<int>(mode);
	processData(jdata);
}

std::shared_ptr<NovelTea::Game> NovelTeaWidget::getGame()
{
	return m_engine->getGame();
}

void NovelTeaWidget::reset()
{
	NovelTea::EngineConfig config;
	config.width = m_internalSize.x;
	config.height = m_internalSize.y;
	config.initialState = NovelTea::StateID::Editor;
	config.fontSizeMultiplier = 0.7f;

	if (m_engine)
		delete m_engine;
	m_engine = new NovelTea::Engine(config);
	m_engine->initialize();
	m_engine->setFramerateLocked(false);
	m_engine->update(0.01f); // This triggers update of state stack
	onResize();
}

void NovelTeaWidget::mousePressEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseButtonPressed;
	event.mouseButton = sf::Event::MouseButtonEvent {sf::Mouse::Button::Left, e->x(), e->y()};
	m_engine->processEvent(event);
	if (getFPS() == 0.f)
		repaint();
}

void NovelTeaWidget::mouseReleaseEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseButtonReleased;
	event.mouseButton = sf::Event::MouseButtonEvent {sf::Mouse::Button::Left, e->x(), e->y()};
	m_engine->processEvent(event);
	if (getFPS() == 0.f)
		repaint();
}

void NovelTeaWidget::mouseMoveEvent(QMouseEvent *e)
{
	sf::Event event;
	event.type = sf::Event::MouseMoved;
	event.mouseMove = sf::Event::MouseMoveEvent {e->x(), e->y()};
	m_engine->processEvent(event);
	if (getFPS() == 0.f)
		repaint();
}

void NovelTeaWidget::onInit()
{
}

void NovelTeaWidget::onResize()
{
	m_engine->resize(getSize().x, getSize().y);
}

void NovelTeaWidget::onUpdate(float delta)
{
	// TODO: fix QT / SFML window message conflict
	sf::Event event;
	while (pollEvent(event))
		std::cout << "pollEvent worked!" << std::endl;

	m_engine->update(delta);
	m_engine->render(*this);

//	std::cout << "onupdate" << std::endl;

	// Change time here according to FPS needs?
//	timer.start(1, this);
}
