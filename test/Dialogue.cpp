#include "Engine.hpp"
#include <NovelTea/DialoguePlayer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

using namespace NovelTea;

class DialogueTest : public EngineTest {
public:
	DialogueTest()
	: player(getContext())
	{

	}
protected:
	void loadDialogue(const std::string &idName) {
		auto dialogue = Proj->get<Dialogue>(idName, getContext());
		player.setDialogue(dialogue);
	}
	DialoguePlayer player;
};

TEST_F(DialogueTest, Empty) {
	EXPECT_TRUE(true);
}
