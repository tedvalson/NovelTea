#ifndef TEST_ENGINE_HPP
#define TEST_ENGINE_HPP

#include <gtest/gtest.h>
#include <NovelTea/Context.hpp>
#include <NovelTea/Engine.hpp>

class EngineTest : public testing::Test {
protected:
	EngineTest();
	void SetUp() override;
	void TearDown() override;
	void loadProject(const std::string &fileName);
	NovelTea::Context *getContext(){ return context; }
	static void resetEngineDir();
	
	static bool initialized;
	NovelTea::ContextConfig config;
	NovelTea::Context *context;
	NovelTea::Engine *engine;
};

#endif // TEST_ENGINE_HPP
