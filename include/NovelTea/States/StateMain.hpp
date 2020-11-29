#ifndef NOVELTEA_STATEMAIN_HPP
#define NOVELTEA_STATEMAIN_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/Tween.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <NovelTea/GUI/Inventory.hpp>
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

class StateMain : public State, public Scrollable
{
public:
	static const int ACTION_BUILDER = 2;

	StateMain(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

	void setMode(Mode mode, const std::string &idName = std::string());
	void setMode(const json &jEntity);

	void showToolbar();
	void hideToolbar();

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	int getValues(int tweenType, float *returnValues) override;
	void setValues(int tweenType, float *newValues) override;

	void processTestSteps();
	bool processAction(const std::string &verbId, const std::vector<std::string> &objectIds);

	bool gotoNextEntity();

	void updateRoomText(const std::string &newText = " ", float duration = 1.f);
	void setActionBuilderShowPos(float position);

protected:
	void callOverlayFunc();
	void repositionText();

private:
	Mode m_mode;
	bool m_testPlaybackMode;
	bool m_testRecordMode;

	// Room
	ActiveText m_roomActiveText;
	ActiveText m_roomActiveTextFadeOut;
	sf::View m_roomTextView;
	float m_roomTextPadding;
	bool m_roomTextChanging;

	float m_scrollPos;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_roomScrollbar;

	TweenRectangleShape m_bgToolbar;
	Button m_buttonSettings;
	Button m_buttonTextLog;

	VerbList m_verbList;
	ActionBuilder m_actionBuilder;
	Inventory m_inventory;
	Navigation m_navigation;
	TextOverlay m_textOverlay;
	std::string m_selectedObjectId;
	DukValue m_textOverlayFunc;

	// Cutscene
	std::shared_ptr<Cutscene> m_cutscene;
	CutsceneRenderer m_cutsceneRenderer;
	float m_cutsceneSpeed;

	// Dialogue
	std::shared_ptr<Dialogue> m_dialogue;
	DialogueRenderer m_dialogueRenderer;

	float m_actionBuilderShowPos;

	TweenEngine::Tween *m_highlightTween;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_HPP
