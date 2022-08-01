#include "Engine.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/FileUtils.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>

using namespace NovelTea;

extern std::string g_dir;

bool EngineTest::initialized = false;

void EngineTest::SetUp()
{
	if (!initialized) {
		initialized = true;
		resetEngineDir();
	}
	
	auto dir = g_dir + "settings";
	GSettings.setDirectory(dir);
	GSettings.load();

	config.width = 480;
	config.height = 700;
//	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.dpiMultiplier = 2.f;
	config.saveDir = dir;
//	config.initialState = StateID::Main;

	engine = new Engine(config);
	engine->initialize();
	engine->setFramerateLocked(false);

	GSave->setDirectory(dir);
}

void EngineTest::TearDown()
{
	delete engine;
}

void EngineTest::loadProject(const std::string &fileName)
{
	ASSERT_TRUE(Proj.loadFromFile(g_dir + fileName)) << fileName;
}

void EngineTest::resetEngineDir()
{
	createDir(g_dir + "settings", true);
}
