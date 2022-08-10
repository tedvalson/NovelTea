#include "Engine.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/FileUtils.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>

using namespace NovelTea;

extern std::string g_dir;

bool EngineTest::initialized = false;

EngineTest::EngineTest()
{
		resetEngineDir();
	

	config.width = 480;
	config.height = 700;
//	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.dpiMultiplier = 2.f;
//	config.initialState = StateID::Main;

	auto dir = g_dir + "settings";
	config.settingsDir = dir;
	config.saveDir = dir;
	
	context = new Context(config);
}

void EngineTest::SetUp()
{
	engine = new Engine(context);
	engine->initialize();
	engine->setFramerateLocked(false);
}

void EngineTest::TearDown()
{
	delete engine;
}

void EngineTest::loadProject(const std::string &fileName)
{
}

void EngineTest::resetEngineDir()
{
	createDir(g_dir + "settings", true);
}
