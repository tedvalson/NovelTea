#include "Engine.hpp"
#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

using namespace NovelTea;

class DialogueTest : public EngineTest {
protected:
	void loadDialogue(const std::string &idName) {
		auto dialogue = GSave->get<Dialogue>(idName);
		dr.setDialogue(dialogue);
	}

	DialogueRenderer dr;
};

TEST_F(DialogueTest, Empty) {
	EXPECT_TRUE(true);
}
