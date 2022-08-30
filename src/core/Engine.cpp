#include <NovelTea/Engine.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/TextInput.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/States/StateIntro.hpp>
#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/States/StateProfileManager.hpp>
#include <NovelTea/States/StateSettings.hpp>
#include <NovelTea/States/StateTextLog.hpp>
#include <NovelTea/States/StateTextSettings.hpp>
#include <NovelTea/States/StateTitleScreen.hpp>
#include <SFML/System/Time.hpp>
#include <chrono>

using namespace std::chrono;

namespace NovelTea
{

Engine::Engine(Context* context)
	: ContextObject(context)
	, m_framerateLocked(false)
	, m_stateStack(new StateStack(context))
{
	m_stateStack->registerState<StateEditor>(StateID::Editor);
	m_stateStack->registerState<StateIntro>(StateID::Intro);
	m_stateStack->registerState<StateMain>(StateID::Main);
	m_stateStack->registerState<StateProfileManager>(StateID::ProfileManager);
	m_stateStack->registerState<StateSettings>(StateID::Settings);
	m_stateStack->registerState<StateTextLog>(StateID::TextLog);
	m_stateStack->registerState<StateTextSettings>(StateID::TextSettings);
	m_stateStack->registerState<StateTitleScreen>(StateID::TitleScreen);
}

int Engine::run()
{
	if (!initialize()) {
		sf::err() << "Failed to initialize NovelTea Engine." << std::endl;
		return 1;
	}

	auto shader = Proj->getShader(ID::shaderPostProcess);
	shader->setUniform("backbuffer", m_renderTexture.getTexture());

	sf::RenderWindow window(sf::VideoMode(GConfig.width, GConfig.height, 16), "NovelTea Launcher");
	window.setDefaultShader(shader.get());

	GTextInput.textInputTrigger = [&window](const std::string &message, int ref) {
		std::string input;
		std::cout << message << std::endl;
		std::cout << "Waiting for input..." << std::endl;
		std::cin >> input;
		std::cout << "Input: \"" << input << "\"" << std::endl;
		GTextInput.callback(input, ref);
		// Purge events received while waiting for standard input
		sf::Event event;
		while (window.pollEvent(event)) {}
	};

	auto active = true;
	auto startTime = getSystemTimeMs();
	while (window.isOpen())
	{
		sf::Event event;
		while (active ? window.pollEvent(event) : window.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				resize(event.size.width, event.size.height);
			else if (event.type == sf::Event::LostFocus)
				active = false;
			else if (event.type == sf::Event::GainedFocus) {
				update(0.f);
				active = true;
			}

			processEvent(event);
		}

		shader->setUniform("time", 0.001f * (getSystemTimeMs() - startTime));
		update();
		render(window);
		window.display();
	}
	return 0;
}

bool Engine::initialize()
{
	if (!getContext()->initialize())
		return false;

	// Seed the script RNG with system time
	ScriptMan->randSeed(getSystemTimeMs());

	m_lastTime = getSystemTimeMs();
	m_deltaPerFrame = 1.f / GConfig.maxFps;

	resize(GConfig.width, GConfig.height);

	m_stateStack->pushState(GConfig.initialState);
	return true;
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

	NotificationMan->setScreenSize(sf::Vector2f(width, height));

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
	GConfig.width = width;
	GConfig.height = height;

	m_stateStack->resize(sf::Vector2f(width, height));
}

void Engine::render(sf::RenderTarget &target)
{
	m_renderTexture.clear(GConfig.backgroundColor);
	m_stateStack->render(m_renderTexture);
	m_renderTexture.display();

	target.clear();
	target.setView(m_view);
	target.draw(m_sprite, sf::BlendNone);
}

void Engine::update()
{
	auto elapsed = sf::milliseconds(getSystemTimeMs() - m_lastTime);
	auto delta = elapsed.asSeconds();
	if (delta < m_deltaPerFrame)
	{
		sf::sleep(sf::seconds(m_deltaPerFrame - delta));
		update(m_deltaPerFrame);
	}
	else if (getFramerateLocked())
		update(m_deltaPerFrame);
	else
		update(delta);
}

void Engine::update(float deltaSeconds)
{
	m_lastTime = getSystemTimeMs();
	if (deltaSeconds > 0.f)
		m_stateStack->update(deltaSeconds);
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
	return m_stateStack->processData(data);
}

size_t Engine::getSystemTimeMs()
{
	auto t = steady_clock::now().time_since_epoch();
	auto ts = duration_cast<milliseconds>(t);
	return ts.count();
}

void Engine::setFramerateLocked(bool locked)
{
	m_framerateLocked = locked;
}

bool Engine::getFramerateLocked() const
{
	return m_framerateLocked;
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
