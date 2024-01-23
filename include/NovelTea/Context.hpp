#ifndef NOVELTEA_CONTEXT_HPP
#define NOVELTEA_CONTEXT_HPP

#include <NovelTea/States/StateIdentifiers.hpp>
#include <NovelTea/Err.hpp>
#include <NovelTea/json.hpp>
#include <memory>

#define CTX        getContext()
#define GSys(x)    CTX->getSubsystem<x>()
#define GConfig    CTX->getConfig()
#define GTextLog   GSys(NovelTea::TextLog)
#define GData      CTX->getData()
#define GGame      GSys(NovelTea::Game)
#define GSave      GGame->getSaveData()
#define GSaveData  GSave->data()
#define GSettings  GGame->getSettings()
#define Proj       GGame->getProjectData()
#define ProjData   Proj->data()
#define EventMan   GSys(NovelTea::EventManager)
#define ScriptMan  GSys(NovelTea::ScriptManager)
#define TimerMan   GSys(NovelTea::TimerManager)
#define NotificationMan GSys(NovelTea::NotificationManager)

namespace NovelTea {

class Subsystem;
class ProjectData;

struct ContextConfig
{
	ContextConfig();
	size_t width;
	size_t height;
	unsigned maxFps;
	unsigned minFps;
	float fontSizeMultiplier;
	float dpiMultiplier;
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
	void update(float delta);

	template <typename T>
	void registerSubsystem(bool replaceExisting = true)
	{
		auto name = T::name();
		if (m_initialized)
			err() << "Cannot register subsystem '" << name << "' after Context is initialized." << std::endl;
		if (!replaceExisting && hasSubsystem(name))
			return;
		m_subsystemFactories[name] = [this](){
			return std::make_shared<T>(this);
		};
	}

	template <typename T>
	std::shared_ptr<T> getSubsystem()
	{
		auto name = T::name();
		// Check if subsystems is empty too because initialize() uses this method (infinite loop)
		if (!m_initialized && m_subsystems.empty()) {
			warn() << "Accessing subsystem '" << name << "' has caused Context to auto-initialize." << std::endl;
			initialize();
		}
		if (m_subsystems.find(name) == m_subsystems.end())
			err() << "Accessing unregistered subsystem '" << name << "'. Using it will crash." << std::endl;
		return std::static_pointer_cast<T>(m_subsystems[name]);
	}

	bool hasSubsystem(const std::string &name);

	ContextConfig &getConfig() { return m_config; }
	sj::JSON &getData() { return m_data; }

private:
	// For compatibility with above defined macros
	Context *getContext() { return this; }

private:
	bool m_initialized;
	ContextConfig m_config;
	sj::JSON m_data;
	std::map<std::string, std::function<std::shared_ptr<Subsystem>()>> m_subsystemFactories;
	std::map<std::string, std::shared_ptr<Subsystem>> m_subsystems;
};

} // namespace NovelTea

#endif // NOVELTEA_CONTEXT_HPP
