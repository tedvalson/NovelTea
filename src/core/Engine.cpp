#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/States/StateIntro.hpp>
#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/States/StateSettings.hpp>
#include <NovelTea/States/StateTextLog.hpp>
#include <NovelTea/States/StateTextSettings.hpp>
#include <NovelTea/States/StateTitleScreen.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>

using namespace std::chrono;

namespace NovelTea
{

Engine::Engine(EngineConfig config)
	: m_config(config)
{
	m_game = std::make_shared<Game>();

	auto stateStack = new StateStack(State::Context(m_config, *m_game, m_data));
	m_stateStack = std::unique_ptr<StateStack>(stateStack);

	m_stateStack->registerState<StateEditor>(StateID::Editor);
	m_stateStack->registerState<StateIntro>(StateID::Intro);
	m_stateStack->registerState<StateMain>(StateID::Main);
	m_stateStack->registerState<StateSettings>(StateID::Settings);
	m_stateStack->registerState<StateTextLog>(StateID::TextLog);
	m_stateStack->registerState<StateTextSettings>(StateID::TextSettings);
	m_stateStack->registerState<StateTitleScreen>(StateID::TitleScreen);
}

void Engine::run()
{

}

bool Engine::isRunning() const
{
	return true;
}

void Engine::resize(size_t width, size_t height)
{
	m_internalRatio = static_cast<float>(width) / height;
	m_view.reset(sf::FloatRect(0, 0, width, height));

	m_renderTexture.create(width, height);
	m_renderTexture.setSmooth(true);
	m_sprite.setTexture(m_renderTexture.getTexture(), true);

	Notification::setScreenSize(sf::Vector2f(width, height));

	sf::FloatRect viewport;
	sf::Vector2f widgetSize(width, height);
	auto ratio = widgetSize.x / widgetSize.y;

	if (ratio < m_internalRatio)
	{
		viewport.width = 1.f;
		viewport.height = ratio / m_internalRatio;
		viewport.left = 0.f;
		viewport.top = (1.f - viewport.height) / 2.f;
	}
	else
	{
		viewport.width = m_internalRatio / ratio;
		viewport.height = 1.f;
		viewport.left = (1.f - viewport.width) / 2.f;
		viewport.top = 0.f;
	}

	m_view.setViewport(viewport);
	m_width = width;
	m_height = height;
	m_config.width = width;
	m_config.height = height;

	m_stateStack->resize(sf::Vector2f(width, height));
}

void Engine::render(sf::RenderTarget &target)
{
	GMan.setActive(m_game);

	m_renderTexture.clear(m_config.backgroundColor);
	m_stateStack->render(m_renderTexture);
	m_renderTexture.display();

	target.clear();
	target.setView(m_view);
	target.draw(m_sprite, sf::BlendNone);
}

void Engine::update()
{
	GMan.setActive(m_game);
	auto time = getSystemTimeMs();
	auto elapsed = sf::milliseconds(time - m_lastTime);
	auto delta = elapsed.asSeconds();
	if (delta < m_deltaPerFrame)
	{
		sf::sleep(sf::seconds(m_deltaPerFrame - delta));
		m_lastTime = getSystemTimeMs();
		m_stateStack->update(m_deltaPerFrame);
	}
	else
	{
		m_lastTime = time;
		m_stateStack->update(delta);
	}
}

void Engine::update(float deltaSeconds)
{
	GMan.setActive(m_game);
	m_lastTime = getSystemTimeMs();
	m_stateStack->update(deltaSeconds);
}

void Engine::processEvent(const sf::Event &event)
{
	GMan.setActive(m_game);
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
			e.type == sf::Event::MouseButtonReleased)
	{
		pos.x = e.mouseButton.x;
		pos.y = e.mouseButton.y;
	}
	else if (e.type == sf::Event::MouseMoved)
	{
		pos.x = e.mouseMove.x;
		pos.y = e.mouseMove.y;
	}

	if (e.type == sf::Event::TouchBegan)
		e.type = sf::Event::MouseButtonPressed;
	else if (e.type == sf::Event::TouchEnded)
		e.type = sf::Event::MouseButtonReleased;
	else if (e.type == sf::Event::TouchMoved)
		e.type = sf::Event::MouseMoved;

	if (e.type == sf::Event::MouseButtonPressed ||
			e.type == sf::Event::MouseButtonReleased)
	{
		e.mouseButton.x = pos.x;
		e.mouseButton.y = pos.y;
	}
	else if (e.type == sf::Event::MouseMoved)
	{
		e.mouseMove.x = pos.x;
		e.mouseMove.y = pos.y;
	}

	m_stateStack->processEvent(e);
}

void *Engine::processData(void *data)
{
	GMan.setActive(m_game);
	return m_stateStack->processData(data);
}

size_t Engine::getSystemTimeMs()
{
	auto t = steady_clock::now().time_since_epoch();
	auto ts = duration_cast<milliseconds>(t);
	return ts.count();
}

std::shared_ptr<Game> Engine::getGame()
{
	return m_game;
}

void Engine::initialize()
{
	GMan.setActive(m_game);
	m_game->initialize();
	m_lastTime = getSystemTimeMs();
	m_deltaPerFrame = 1.f / m_config.fps;

	resize(m_config.width, m_config.height);

	m_stateStack->pushState(m_config.initialState);
}

sf::Vector2f Engine::mapPixelToCoords(const sf::Vector2i &point) const
{
	// First, convert from viewport coordinates to homogeneous coordinates
	sf::Vector2f normalized;

	float width  = static_cast<float>(m_width);
	float height = static_cast<float>(m_height);
	const sf::FloatRect& viewport = m_view.getViewport();

	auto viewRect = sf::IntRect(static_cast<int>(0.5f + width  * viewport.left),
				static_cast<int>(0.5f + height * viewport.top),
				static_cast<int>(0.5f + width  * viewport.width),
				static_cast<int>(0.5f + height * viewport.height));

	normalized.x = -1.f + 2.f * (point.x - viewRect.left) / viewRect.width;
	normalized.y =  1.f - 2.f * (point.y - viewRect.top)  / viewRect.height;

	// Then transform by the inverse of the view matrix
	return m_view.getInverseTransform().transformPoint(normalized);
}

} // namespace NovelTea
