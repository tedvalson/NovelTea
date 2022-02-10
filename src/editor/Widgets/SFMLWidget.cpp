#include "SFMLWidget.h"
#include <QFile>
#include <iostream>
#include <chrono>

using namespace std::chrono;

SFMLWidget::SFMLWidget(QWidget *parent):
	QWidget(parent),
	_initialized(false)
{
	// Setup some states to allow direct rendering into the widget
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);

	// Set strong focus to enable keyboard events to be received
	setFocusPolicy(Qt::StrongFocus);

	setFPS(60.f);
}

SFMLWidget::~SFMLWidget()
{

}

QByteArray SFMLWidget::getData(QString filepath)
{
	QFile file(filepath);
	file.open(QIODevice::ReadOnly);
	return file.readAll();
}

int SFMLWidget::getTimeMs()
{
	auto t = steady_clock::now().time_since_epoch();
	auto ts = duration_cast<milliseconds>(t);
	return ts.count();
}

void SFMLWidget::onResize()
{

}

void SFMLWidget::showEvent(QShowEvent *)
{
//	std::cout << "SFML showEvent " << width() << ", " << height() << std::endl;
	if (!_initialized)
	{
		std::cout << "initialized!" << std::endl;
		takeWindow();
		onInit();
//		_timer.start();
		_initialized = true;
	}

	_timer.start();
	_lastTime = getTimeMs();
	setActive();
}

void SFMLWidget::hideEvent(QHideEvent *)
{
	_timer.stop();
//	std::cout << "SFML hide" << std::endl;
}

void SFMLWidget::resizeEvent(QResizeEvent *)
{
	if (!_initialized)
		return;

//	std::cout << "SFML resize: " << QWidget::width() << ", " << QWidget::height() << std::endl;

	// Need to enforce an even size
	auto w = QWidget::width();
	auto h = QWidget::height();
	if (w % 2)
		w--;
	if (h % 2)
		h--;

	if (w == QWidget::width() && h == QWidget::height())
	{
		takeWindow();
		onResize();
	}
	else
		resize(w, h);
}

void SFMLWidget::setFPS(float fps)
{
	m_fps = fps;
	_timer.stop();
	_timer.disconnect();
	if (fps > 0.f) {
		connect(&_timer, SIGNAL(timeout()), this, SLOT(repaint()));
//		_timer.setInterval(1000.f / fps);
		_timer.start(1000.f / fps);
	}
}

float SFMLWidget::getFPS() const
{
	return m_fps;
}

void SFMLWidget::takeWindow()
{
#if defined(_WIN32)
	sf::RenderWindow::create(reinterpret_cast<sf::WindowHandle>(winId()));
#elif defined(__APPLE__) && defined(__MACH__)
	sf::RenderWindow::create(reinterpret_cast<sf::WindowHandle>(winId()));
#else
	sf::RenderWindow::create(static_cast<sf::WindowHandle>(winId()));
#endif
}

void SFMLWidget::paintEvent(QPaintEvent *)
{
	if (!_initialized)
		return;

	auto curTime = getTimeMs();
	auto delta = 0.001f * (curTime - _lastTime);
	_lastTime = curTime;

	onUpdate(delta);
	display();
}

QPaintEngine *SFMLWidget::paintEngine() const
{
	return 0;
}
