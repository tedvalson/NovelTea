#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateMain.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>

using namespace std::chrono;

namespace NovelTea
{

Engine::Engine(EngineConfig config)
: _config(config)
, _stateStack(State::Context(_config, _text, _data))
{
	_stateStack.registerState<StateMain>(StateID::Main);
}

void Engine::run()
{

}

bool Engine::isRunning() const
{
	return true;
}

void Engine::resize(unsigned int width, unsigned int height)
{
	sf::FloatRect viewport;
	sf::Vector2f widgetSize(width, height);
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
	_width = width;
	_height = height;
}

void Engine::render(sf::RenderTarget &target)
{
	target.clear();
	target.setView(_view);
	target.draw(_bg);
	_stateStack.render(target);
}

void Engine::update()
{
	auto time = getSystemTimeMs();
	auto elapsed = sf::milliseconds(time - _lastTime);
	auto delta = elapsed.asSeconds();
	if (delta < _deltaPerFrame)
	{
		sf::sleep(sf::seconds(_deltaPerFrame - delta));
		_lastTime = getSystemTimeMs();
		_stateStack.update(_deltaPerFrame);
	}
	else
	{
		_lastTime = time;
		_stateStack.update(delta);
	}
}

void Engine::update(float deltaSeconds)
{
	_lastTime = getSystemTimeMs();
	_stateStack.update(deltaSeconds);
}

void Engine::processEvent(const sf::Event &event)
{
	auto e = event;
	sf::Vector2i pos;

	// Convert Touch events to mouse events for now.
	// TODO: Check platform using preprocessor to handle Touch events
	if (e.type == sf::Event::TouchBegan ||
			e.type == sf::Event::TouchEnded ||
			e.type == sf::Event::TouchMoved)
	{
		pos.x = e.touch.x;
		pos.y = e.touch.y;
	}
	else if (e.type == sf::Event::MouseButtonPressed ||
			e.type == sf::Event::MouseButtonReleased ||
			e.type == sf::Event::MouseMoved)
	{
		pos.x = e.mouseButton.x;
		pos.y = e.mouseButton.y;
	}

	if (e.type == sf::Event::TouchBegan)
		e.type = sf::Event::MouseButtonPressed;
	else if (e.type == sf::Event::TouchEnded)
		e.type = sf::Event::MouseButtonReleased;
	else if (e.type == sf::Event::TouchMoved)
		e.type = sf::Event::MouseMoved;

	if (e.type == sf::Event::MouseButtonPressed ||
			e.type == sf::Event::MouseButtonReleased ||
			e.type == sf::Event::MouseMoved)
	{
		auto coords = mapPixelToCoords(pos);
		e.mouseButton.x = coords.x;
		e.mouseButton.y = coords.y;
	}

	_stateStack.processEvent(e);
}

void *Engine::processData(void *data)
{
	return _stateStack.processData(data);
}

unsigned int Engine::getSystemTimeMs()
{
	auto t = steady_clock::now().time_since_epoch();
	auto ts = duration_cast<milliseconds>(t);
	return ts.count();
}

void Engine::initialize()
{
	_lastTime = getSystemTimeMs();
	_deltaPerFrame = 1.f / _config.fps;
	_internalRatio = static_cast<float>(_config.width) / _config.height;
	_view.reset(sf::FloatRect(0, 0, _config.width, _config.height));

	_bg.setSize(sf::Vector2f(_config.width, _config.height));
	_bg.setFillColor(sf::Color::White);

	resize(_config.width, _config.height);

	_stateStack.pushState(_config.initialState);
}

sf::Vector2f Engine::mapPixelToCoords(const sf::Vector2i &point) const
{
	// First, convert from viewport coordinates to homogeneous coordinates
	sf::Vector2f normalized;

	float width  = static_cast<float>(_width);
	float height = static_cast<float>(_height);
	const sf::FloatRect& viewport = _view.getViewport();

	auto viewRect = sf::IntRect(static_cast<int>(0.5f + width  * viewport.left),
				static_cast<int>(0.5f + height * viewport.top),
				static_cast<int>(0.5f + width  * viewport.width),
				static_cast<int>(0.5f + height * viewport.height));

	normalized.x = -1.f + 2.f * (point.x - viewRect.left) / viewRect.width;
	normalized.y =  1.f - 2.f * (point.y - viewRect.top)  / viewRect.height;

	// Then transform by the inverse of the view matrix
	return _view.getInverseTransform().transformPoint(normalized);
}

} // namespace NovelTea
