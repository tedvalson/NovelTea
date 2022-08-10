#ifndef NOVELTEA_CONTEXT_HPP
#define NOVELTEA_CONTEXT_HPP

#include <NovelTea/States/StateIdentifiers.hpp>
#include <NovelTea/json.hpp>
#include <SFML/Graphics/Color.hpp>
#include <memory>

#define CTX        getContext()
#define GConfig    CTX->getConfig()
#define GTextLog   CTX->getTextLog()
#define GData      CTX->getData()
#define GGame      CTX->getGame()
#define GSave      GGame->getSaveData()
#define GSaveData  GSave->data()
#define GSettings  GGame->getSettings()
#define Proj       GGame->getProjectData()
#define ProjData   Proj->data()
#define ScriptMan  CTX->getScriptManager()
#define TimerMan   CTX->getTimerManager()
#define NotificationMan CTX->getNotificationManager()

namespace NovelTea {

class Game;
class ProjectData;
class ScriptManager;
class NotificationManager;
class TextLog;
class TimerManager;

struct ContextConfig
{
	ContextConfig();
	size_t width;
	size_t height;
	unsigned maxFps;
	unsigned minFps;
	float fontSizeMultiplier;
	float dpiMultiplier;
	sf::Color backgroundColor;
	StateID initialState;
//	EngineOrientation orientation;
	std::string saveDir;
	std::string settingsDir;
	std::string projectFileName;
	std::shared_ptr<ProjectData> projectData;
	sj::JSON entryEntity;
	sj::JSON entryMeta;
	bool entityPreview;
};

class Context
{
public:
	Context(const ContextConfig &config = ContextConfig());

	bool initialize();

	ContextConfig &getConfig() { return m_config; }
	sj::JSON &getData() { return m_data; }
	std::shared_ptr<Game> getGame() const { return m_game; }
	std::shared_ptr<ScriptManager> getScriptManager() const { return m_scriptManager; }
	std::shared_ptr<TimerManager> getTimerManager() const { return m_timerManager; }
	std::shared_ptr<NotificationManager> getNotificationManager() const { return m_notificationManager; }
	std::shared_ptr<TextLog> getTextLog() const { return m_textLog; }

private:
	// For compatibility with above defined macros
	Context *getContext() { return this; }

private:
	bool m_initialized;
	ContextConfig m_config;
	sj::JSON m_data;
	std::shared_ptr<Game> m_game;
	std::shared_ptr<TimerManager> m_timerManager;
	std::shared_ptr<NotificationManager> m_notificationManager;
	std::shared_ptr<ScriptManager> m_scriptManager;
	std::shared_ptr<TextLog> m_textLog;
};

} // namespace NovelTea

#endif // NOVELTEA_CONTEXT_HPP
