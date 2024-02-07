#include <NovelTea/Context.hpp>
#include <NovelTea/AssetLoader.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Event.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/Notification.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/ScriptManager.hpp>

namespace NovelTea
{

ContextConfig::ContextConfig()
	: width(1024)
	, height(720)
	, maxFps(30)
	, minFps(1)
	, fontSizeMultiplier(0.f) // (0) will use Settings value
	, dpiMultiplier(1.f)
	, initialState(StateID::Intro)
//	, orientation(EngineOrientation::Auto)
//	, saveDir("")
//	, settingsDir("")
//	, projectFileName("")
	, projectData(nullptr)
	, entryEntity(sj::Array())
	, entryMeta(sj::Array())
	, entityPreview(false)
	, useStateEventManager(true)
{

}

Context::Context(const ContextConfig &config)
	: m_initialized(false)
	, m_config(config)
{
	registerSubsystem<AssetLoader>();
	registerSubsystem<EventManager>();
	registerSubsystem<Game>();
	registerSubsystem<TimerManager>();
	registerSubsystem<NotificationManager>();
	registerSubsystem<ScriptManager>();
	registerSubsystem<TextLog>();
	if (config.useStateEventManager)
		registerSubsystem<StateEventManager>();
}

bool Context::initialize()
{
	if (m_initialized)
		return true;

	for (auto& factory : m_subsystemFactories)
		m_subsystems[factory.first] = factory.second();

	GGame->initialize();

	for (auto& subsystem : m_subsystems)
		if (!subsystem.second->initialize()) {
			err() << "Failed to initialized subsystem '" << subsystem.first << "'" << std::endl;
			return false;
		}

	if (m_config.fontSizeMultiplier <= 0.f)
		m_config.fontSizeMultiplier = GSettings->getFontSizeMultiplier();

	// Run the "after load" script when "previewing" even though no real game load happened
	if (m_config.entityPreview)
		ScriptMan->runInClosure(ProjData[NovelTea::ID::scriptAfterLoad].ToString());

	m_initialized = true;
	return m_initialized;
}

void Context::update(float delta)
{
	for (auto& subsystem : m_subsystems)
		subsystem.second->update(delta);
}

bool Context::hasSubsystem(const std::string &name)
{
	return m_subsystemFactories.find(name) != m_subsystemFactories.end();
}
	
} // namespace NovelTea
