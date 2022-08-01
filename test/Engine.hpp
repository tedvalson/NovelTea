#ifndef TEST_ENGINE_HPP
#define TEST_ENGINE_HPP

#include <gtest/gtest.h>
#include <NovelTea/Engine.hpp>

class EngineTest : public testing::Test {
protected:
	void SetUp() override;
	void TearDown() override;
	void loadProject(const std::string &fileName);
	static void resetEngineDir();
	
	static bool initialized;
	NovelTea::EngineConfig config;
	NovelTea::Engine* engine;
};

#endif // TEST_ENGINE_HPP
