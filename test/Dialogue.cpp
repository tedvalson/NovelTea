#include "Engine.hpp"
#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <iostream>

using namespace NovelTea;

class DialogueTest : public EngineTest {
public:
	DialogueTest()
	: dr(getContext())
	{
		
	}
protected:
	void loadDialogue(const std::string &idName) {
		auto dialogue = Proj->get<Dialogue>(idName, getContext());
		dr.setDialogue(dialogue);
	}
	DialogueRenderer dr;
};

TEST_F(DialogueTest, Empty) {
	EXPECT_TRUE(true);
}
