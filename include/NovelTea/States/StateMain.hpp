#ifndef NOVELTEA_STATEMAIN_HPP
#define NOVELTEA_STATEMAIN_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/Tween.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/DialogueRenderer.hpp>
#include <NovelTea/MapRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <NovelTea/GUI/Inventory.hpp>
#include <NovelTea/GUI/Navigation.hpp>
#include <NovelTea/GUI/TextOverlay.hpp>
#include <NovelTea/GUI/Icon/IconGlow.hpp>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace NovelTea
{

class StateMain : public State, public Scrollable
{
public:
	static const int ACTION_BUILDER = 2;

	StateMain(StateStack& stack, Context& context, StateCallback callback);
	~StateMain();
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setMode(EntityMode mode, const std::string &idName = std::string());

	void showToolbar(float duration = 0.5f);
	void hideToolbar(float duration = 0.5f);

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	int getValues(int tweenType, float *returnValues) override;
	void setValues(int tweenType, float *newValues) override;

	bool processAction(const std::string &verbId, const std::vector<std::string> &objectIds);


	void updateUI();
	void updateRoomText(const std::string &newText = " ", float duration = 1.f);
	void setActionBuilderShowPos(float position);

protected:
	void callOverlayFunc();
	void repositionText();
	void quit();

private:
	EntityMode m_mode;
	bool m_testPlaybackMode;
	bool m_testRecordMode;
	bool m_quitting;

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
	Button m_buttonInventory;
	Button m_buttonSettings;
	Button m_buttonTextLog;

	VerbList m_verbList;
	ActionBuilder m_actionBuilder;
	Inventory m_inventory;
	Navigation m_navigation;
	TextOverlay m_textOverlay;
	std::string m_selectedObjectId;
	DukValue m_textOverlayFunc;
	TweenRectangleShape m_bg;

	IconGlow m_iconSave;

	sf::Clock m_clock;
	bool m_quickVerbPressed;

	int m_eventListenerId;

	// Cutscene
	CutsceneRenderer m_cutsceneRenderer;
	float m_cutsceneSpeed;

	// Dialogue
	DialogueRenderer m_dialogueRenderer;

	// Map
	MapRenderer m_mapRenderer;

	float m_actionBuilderShowPos;

	TweenEngine::TweenManager m_tweenManager;
	TweenEngine::TweenManager m_tweenManagerHighlights;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_HPP
