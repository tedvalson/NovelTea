#include <NovelTea/Context.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/GUI/Notification.hpp>
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
	, backgroundColor(sf::Color(200, 200, 200))
	, initialState(StateID::Intro)
//	, orientation(EngineOrientation::Auto)
//	, saveDir("")
//	, settingsDir("")
//	, projectFileName("")
	, projectData(nullptr)
	, entryEntity(sj::Array())
	, entryMeta(sj::Array())
	, entityPreview(false)
{

}

Context::Context(const ContextConfig &config)
	: m_initialized(false)
	, m_config(config)
	, m_game(new Game(this))
	, m_timerManager(new TimerManager(this))
	, m_notificationManager(new NotificationManager(this))
	, m_scriptManager(new ScriptManager(this))
	, m_textLog(new TextLog)
{

}

bool Context::initialize()
{
	if (m_initialized)
		return true;

	m_game->initialize();

	if (m_config.fontSizeMultiplier <= 0.f)
		m_config.fontSizeMultiplier = GSettings->getFontSizeMultiplier();

	// Run the "after load" script when "previewing" even though no real game load happened
	if (m_config.entityPreview)
		m_scriptManager->runInClosure(ProjData[NovelTea::ID::scriptAfterLoad].ToString());

	m_initialized = true;
	return true;
}
	
} // namespace NovelTea
