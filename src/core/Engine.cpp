#include <NovelTea/Engine.hpp>
#include <NovelTea/Event.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/TextInput.hpp>
#include <thread>
#include <chrono>

using namespace std::chrono;

namespace NovelTea
{

Engine::Engine(Context* context)
: ContextObject(context)
, m_initialized(false)
, m_framerateLocked(false)
{
}

Engine::~Engine()
{
}

int Engine::run()
{
	if (!initialize()) {
		err() << "Failed to initialize NovelTea Engine." << std::endl;
		return 1;
	}

	GTextInput.textInputTrigger = [](const std::string &message, int ref) {
		std::string input;
		std::cout << message << std::endl;
		std::cout << "Waiting for input..." << std::endl;
		std::cin >> input;
		std::cout << "Input: \"" << input << "\"" << std::endl;
		GTextInput.callback(input, ref);
	};

	auto active = true;
	auto startTime = getSystemTimeMs();
	while (active)
	{
		update();
	}
	return 0;
}

bool Engine::initialize()
{
	if (m_initialized)
		return true;
	if (!getContext()->initialize())
		return false;

	// Seed the script RNG with system time
	ScriptMan->randSeed(getSystemTimeMs());

	m_lastTime = getSystemTimeMs();
	m_deltaPerFrame = 1.f / GConfig.maxFps;

	m_initialized = true;
	return true;
}

bool Engine::isRunning() const
{
	return true;
}

void Engine::update()
{
	auto elapsed = getSystemTimeMs() - m_lastTime;
	auto delta = 0.001f * elapsed;
	if (delta < m_deltaPerFrame)
	{
		auto ms = static_cast<int>(1000.f * (m_deltaPerFrame - delta));
		std::this_thread::sleep_for(milliseconds(ms));
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
		getContext()->update(deltaSeconds);
}

std::shared_ptr<EventManager> Engine::events()
{
	return EventMan;
}

int64_t Engine::getSystemTimeMs()
{
	auto t = steady_clock::now().time_since_epoch();
	auto ts = duration_cast<milliseconds>(t);
	return ts.count();
}

} // namespace NovelTea
