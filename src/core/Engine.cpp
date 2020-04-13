#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateMain.hpp>

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

}

void Engine::render(sf::RenderTarget &target)
{
	_stateStack.render(target);
}

void Engine::update(float deltaSeconds)
{
	_stateStack.update(deltaSeconds);
}

void Engine::processEvent(const sf::Event &event)
{
	_stateStack.processEvent(event);
}

void *Engine::processData(void *data)
{
	return _stateStack.processData(data);
}

void Engine::initialize()
{
	_deltaPerFrame = 1000.f / _config.fps;

	resize(_config.width, _config.height);

	_stateStack.pushState(_config.initialState);
}

} // namespace NovelTea
