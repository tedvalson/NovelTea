#ifndef NOVELTEA_STATEMAIN_HPP
#define NOVELTEA_STATEMAIN_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <NovelTea/GUI/Navigation.hpp>
#include <NovelTea/GUI/TextOverlay.hpp>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace NovelTea
{

using TestCallback = std::function<bool(const json&)>;

enum class Mode {
	Nothing,
	Cutscene,
	Dialogue,
	Room,
};

class StateMain : public State
{
public:
	StateMain(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

	void setMode(Mode mode, const std::string &idName = std::string());
	void setMode(const json &jEntity);

	void processTestSteps();
	bool processAction(const std::string &verbId, const std::vector<std::string> &objectIds);

	void gotoNextEntity();

	void updateRoomText(const std::string &newText = " ");

protected:
	void callOverlayFunc();

private:
	Mode m_mode;
	bool m_testPlaybackMode;
	bool m_testRecordMode;

	ActiveText m_roomActiveText;
	ActiveText m_roomActiveTextFadeOut;
	bool m_roomTextChanging;

	VerbList m_verbList;
	ActionBuilder m_actionBuilder;
	Navigation m_navigation;
	TextOverlay m_textOverlay;
	std::string m_selectedObjectId;
	DukValue m_textOverlayFunc;

	std::shared_ptr<Cutscene> m_cutscene;
	CutsceneRenderer m_cutsceneRenderer;
	float m_cutsceneSpeed;

	std::shared_ptr<Dialogue> m_dialogue;
	DialogueRenderer m_dialogueRenderer;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_HPP
