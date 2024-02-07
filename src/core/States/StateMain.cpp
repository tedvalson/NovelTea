#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/SFML/AssetLoaderSFML.hpp>
#include <NovelTea/SFML/NotificationSFML.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateMain::StateMain(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_mode(ModeNothing)
, m_testPlaybackMode(false)
, m_testRecordMode(false)
, m_quitting(false)
, m_roomActiveText(&context)
, m_roomActiveTextFadeOut(&context)
, m_roomTextChanging(false)
, m_scrollPos(0.f)
, m_buttonInventory(&context)
, m_buttonSettings(&context)
, m_buttonTextLog(&context)
, m_verbList(&context)
, m_actionBuilder(&context)
, m_inventory(&context)
, m_navigation(&context)
, m_textOverlay(&context)
, m_iconSave(&context)
, m_quickVerbPressed(false)
, m_cutsceneRenderer(&context)
, m_cutsceneSpeed(1.f)
, m_dialogueRenderer(&context)
, m_mapRenderer(&context)
{
	m_eventListenerId = EventMan->listen([this](const EventPtr& event){
		auto type = event->type();
		if (type == Event::TimerCompleted) {
			if (m_mode == ModeRoom)
				updateUI();
		}
		else if (type == StateEvent::Message) {
			auto data = std::static_pointer_cast<StateEvent::MessageEvent>(event);
			if (!m_testPlaybackMode) {
				m_textOverlay.setTextArray(data->messages);
				m_textOverlay.show();
				hideToolbar(0.4f);
			}
			else
				callOverlayFunc();
		}
		else if (type == StateEvent::ModeChanged) {
			setMode(static_cast<EntityMode>(event->intVal), "");
		}
		else if (type == StateEvent::CutsceneChanged) {
			auto cutscene = std::static_pointer_cast<Cutscene>(event->object);
			m_cutsceneRenderer.setCutscene(cutscene);
		}
		else if (type == StateEvent::RoomTextChanged) {
			updateRoomText(event->text);
		}
		else if (type == Event::GameSaving) {
			if (m_mode == ModeCutscene) {
				auto metaData = sj::Array(GGame->getRoom()->getId());
				metaData.append(m_cutsceneRenderer.saveState());
				GSaveData[ID::entrypointMetadata] = metaData;
			}
		}
		else if (type == Event::GameSaved) {
			m_iconSave.show(0.4f, 3.f);
		}
		else if (type == Event::GameLoaded) {
			if (m_mode == ModeCutscene) {
				auto &entryMetadata = GSaveData[ID::entrypointMetadata];
				if (entryMetadata.size() > 1)
					m_cutsceneRenderer.restoreState(entryMetadata[1]);
			}
		}
		else if (type == StateEditor::ModeChanged) {
			requestStackClear();
			requestStackPush(StateID::Editor);
			EventMan->push(event);
		}
		else if (type == StateEditor::RunTest) {
			auto& jsonData = event->json;
		}
		return true;
	});

	auto &text = m_iconSave.getText();
	text.setString(L"\uf0c7");
	text.setFillColor(sf::Color(160, 160, 160, 0));
	m_iconSave.setSpeed(2.f);

	// Room
	m_roomScrollbar.setColor(sf::Color(0, 0, 0, 40));
	m_roomScrollbar.setAutoHide(false);
	m_roomScrollbar.attachObject(this);

	// Navigation setup
	// Set all Navigation transforms before getGlobalBounds is called
	m_navigation.setCallback([this](int direction, const json &jentity){
		if (m_testRecordMode) {
			json jtestItem({
				"type", "move",
				"direction", direction
			});
			runCallback(&jtestItem);
		}
		GGame->pushNextEntityJson(jentity);
	});

	// Toolbar
	m_bgToolbar.setFillColor(sf::Color(0, 0, 0, 0));

	m_bg.setFillColor(sf::Color(200, 200, 200));

	m_buttonInventory.getText().setFont(*Asset->font("sysIcon"));
	m_buttonInventory.setString(L"\uf0b1");
	m_buttonInventory.setAlpha(0.f);
	m_buttonInventory.setActiveColor(sf::Color(0, 0, 0, 50));
	m_buttonInventory.setColor(sf::Color(0, 0, 0, 30));
	m_buttonInventory.onClick([this](){
		if (m_inventory.isOpen())
			m_inventory.close();
		else
			m_inventory.open();
	});

	m_buttonSettings = m_buttonInventory;
	m_buttonSettings.setString(L"\uf013");
	m_buttonSettings.setColor(sf::Color::Transparent);
	m_buttonSettings.setActiveColor(sf::Color(0, 0, 0, 30));
	m_buttonSettings.setTextColor(sf::Color(0, 0, 0, 200));
	m_buttonSettings.setTextActiveColor(sf::Color(0, 0, 0, 255));
	m_buttonSettings.onClick([this](){
		requestStackPush(StateID::Settings);
	});

	m_buttonTextLog = m_buttonSettings;
	m_buttonTextLog.setString(L"\uf02d");
	m_buttonTextLog.onClick([this](){
		requestStackPush(StateID::TextLog);
	});

	// Inventory setup
	m_inventory.setCallback([this](const std::string &objectId, float posX, float posY){
		if (m_actionBuilder.isVisible()) {
			m_actionBuilder.setObject(objectId);
			m_actionBuilder.selectNextEmptyIndex();
			m_inventory.close();
		} else {
			m_selectedObjectId = objectId;
			m_verbList.setVerbs(objectId);
			auto p = sf::Vector2f(posX - m_verbList.getLocalBounds().width, posY - m_verbList.getLocalBounds().height);
			m_verbList.setPositionBounded(p);
			m_verbList.show();
		}
	});

	// VerbList setup
	m_verbList.setSelectCallback([this](const std::string &verbId){
		m_actionBuilder.setVerb(verbId);
		m_actionBuilder.setObject(m_selectedObjectId, 0);
		m_actionBuilder.selectNextEmptyIndex();
		m_verbList.hide();
		m_inventory.close();
		if (!m_actionBuilder.isVisible() && m_actionBuilder.getObjects().size() > 1)
			TweenEngine::Tween::to(*this, ACTION_BUILDER, 0.4f)
				.target(1.f)
				.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
					m_actionBuilder.show();
				})
				.start(m_tweenManager);
	});
	m_verbList.setShowHideCallback([this](bool showing){
		if (!showing) {
			m_roomActiveText.setHighlightId("");
		}
	});

	// ActionBuilder setup
	m_actionBuilder.setCallback([this](bool confirmed){
		if (confirmed)
			processAction(m_actionBuilder.getVerb(), m_actionBuilder.getObjects());
		m_actionBuilder.hide(0.4f, ActionBuilder::ALPHA, [this](){
			TweenEngine::Tween::to(*this, ACTION_BUILDER, 0.4f)
				.target(0.f)
				.start(m_tweenManager);
		});
		m_inventory.close();
	});
	setActionBuilderShowPos(0.f);

	// Dialogue setup
	m_dialogueRenderer.hide(0.f);

	// Map setup
	m_mapRenderer.setMiniMapMode(true);

	m_textOverlay.hide(0.f);

	hideToolbar(0.f);

	SEM->reset();
}

StateMain::~StateMain()
{
	EventMan->remove(m_eventListenerId);
}

void StateMain::render(sf::RenderTarget &target)
{
	static auto shader = Asset->shader(ID::shaderBackground);
	target.draw(m_bg, shader.get());

	if (m_mode != ModeRoom && m_roomTextChanging)
	{
		auto view = target.getView();
		target.setView(m_roomTextView);
		target.draw(m_roomActiveTextFadeOut);
		target.setView(view);
	}

	if (m_mode == ModeCutscene)
	{
		target.draw(m_cutsceneRenderer);
	}
	else if (m_mode == ModeDialogue)
	{
	}
	else if (m_mode == ModeRoom)
	{
		auto view = target.getView();
		target.setView(m_roomTextView);
		target.draw(m_roomActiveTextFadeOut);
		target.draw(m_roomActiveText);
		target.setView(view);
		if (m_actionBuilder.isVisible())
			target.draw(m_actionBuilder);
		target.draw(m_roomScrollbar);
	}

	target.draw(m_dialogueRenderer);
	target.draw(m_bgToolbar);
	target.draw(m_inventory);
	target.draw(m_navigation);

	if (!GGame->getObjectList()->items().empty())
		target.draw(m_buttonInventory);
	target.draw(m_mapRenderer);

	target.draw(m_textOverlay);

	target.draw(m_buttonSettings);
	target.draw(m_buttonTextLog);
	if (m_verbList.isVisible())
		target.draw(m_verbList);

	target.draw(m_iconSave);
	target.draw(*GSys(NotificationManagerSFML));
}

void StateMain::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto wi = std::min(w, h);
	auto portrait = (h > w);
	auto toolbarHeight = round(std::max(w, h) * 2.f/9.f);
	auto toolbarPadding = toolbarHeight / 10.f;

	// Room
	auto fontSizeMultiplier = GConfig.fontSizeMultiplier * GConfig.dpiMultiplier;
	m_roomScrollbar.setPosition(w - 4.f, 4.f);
	m_roomTextPadding = round(1.f / 16.f * wi);
	m_roomActiveText.setFontSizeMultiplier(fontSizeMultiplier);
	m_roomActiveText.setLineSpacing(fontSizeMultiplier * 5.f);
	m_roomActiveText.setSize(sf::Vector2f((portrait ? 1.f : 0.6f) * w - m_roomTextPadding*2, 0.f));

	m_roomActiveTextFadeOut.setFontSizeMultiplier(fontSizeMultiplier);
	m_roomActiveTextFadeOut.setLineSpacing(fontSizeMultiplier * 5.f);

	// Cutscene
	m_cutsceneRenderer.setMargin(m_roomTextPadding);
	m_cutsceneRenderer.setSize(size);
	m_cutsceneRenderer.setFontSizeMultiplier(fontSizeMultiplier);

	m_navigation.setSize(sf::Vector2f(toolbarHeight - toolbarPadding*2, toolbarHeight - toolbarPadding*2));
	m_navigation.setPosition(toolbarPadding, toolbarPadding + h - toolbarHeight);

	// Toolbar
	m_bgToolbar.setSize(sf::Vector2f(w, portrait ? toolbarHeight : 0.f));
	m_bgToolbar.setPosition(0.f, h - toolbarHeight);

	m_buttonSettings.getText().setCharacterSize(0.15f * toolbarHeight);
	m_buttonSettings.setSize(toolbarHeight/5, toolbarHeight/5);
	m_buttonSettings.setPosition(w - toolbarPadding - m_buttonSettings.getSize().x * 1.2f, h - m_buttonSettings.getSize().y - toolbarPadding);

	m_buttonTextLog.getText().setCharacterSize(0.15f * toolbarHeight);
	m_buttonTextLog.setSize(m_buttonSettings.getSize());
	m_buttonTextLog.setPosition(m_buttonSettings.getPosition());
	m_buttonTextLog.move(-m_buttonSettings.getSize().x, 0.f);

	m_buttonInventory.getText().setCharacterSize(0.3f * toolbarHeight);
	m_buttonInventory.setSize(toolbarHeight/2, toolbarHeight/2);
	m_buttonInventory.setPosition(w - toolbarPadding - m_buttonInventory.getSize().x, m_buttonSettings.getPosition().y - m_buttonInventory.getSize().y * 1.2f);

	m_inventory.setScreenSize(size);
	m_inventory.setFontSizeMultiplier(fontSizeMultiplier);
	m_inventory.setStartPosition(sf::Vector2f(w, m_buttonInventory.getPosition().y));
	m_inventory.refreshItems();

	// Notification setup
	GSys(NotificationManagerSFML)->setScreenSize(size);
	GSys(NotificationManagerSFML)->setFontSizeMultiplier(fontSizeMultiplier);

	m_textOverlay.setFontSizeMultiplier(fontSizeMultiplier);

	m_actionBuilder.setSize(sf::Vector2f((portrait ? 1.f : 0.5f) * w, portrait ? 0.25f * h : 0.3f * h));
	m_actionBuilder.setPosition(portrait ? 0.f : (w - m_roomActiveText.getSize().x)/2, h);

	// Dialogue
	m_dialogueRenderer.setSize(size);
	m_dialogueRenderer.setFontSizeMultiplier(fontSizeMultiplier);

	// Map
	auto navSize = m_navigation.getSize();
	m_mapRenderer.setMiniMapSize(navSize);
	m_mapRenderer.setSize(size);
	if (portrait)
		m_mapRenderer.setMiniMapPosition(sf::Vector2f(toolbarPadding*2 + navSize.x, m_navigation.getPosition().y));
	else
		m_mapRenderer.setMiniMapPosition(sf::Vector2f(toolbarPadding, m_navigation.getPosition().y - navSize.y));

	m_textOverlay.setSize(size);

	m_bg.setSize(size);

	m_verbList.setScreenSize(size);
	m_verbList.setFontSizeMultiplier(fontSizeMultiplier);
	m_verbList.hide(0.f);

	auto iconHeight = wi * 0.05f;
	m_iconSave.getText().setCharacterSize(iconHeight);
	m_iconSave.setPosition(w - iconHeight, iconHeight);

	setActionBuilderShowPos(m_actionBuilderShowPos);
	m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
	updateScrollbar();
	m_roomScrollbar.setScroll(0.f);
}

void StateMain::setMode(EntityMode mode, const std::string &idName)
{
	m_roomActiveText.setHighlightId("");
	m_actionBuilder.hide(0.4f, ActionBuilder::ALPHA, [this](){
		TweenEngine::Tween::to(*this, ACTION_BUILDER, 0.4f)
			.target(0.f)
			.start(m_tweenManager);
	});
	m_verbList.hide();

	if (mode != ModeRoom)
	{
		updateRoomText("");
		hideToolbar();
	}
	if (mode != ModeDialogue)
		m_dialogueRenderer.hide();

	if (mode == ModeCutscene)
	{
	}
	else if (mode == ModeDialogue)
	{
		m_dialogueRenderer.show();
	}
	else if (mode == ModeRoom)
	{
			// Should be set after Game room is changed, for map script eval purposes
		m_mode = mode;
		showToolbar();
		updateUI();
		m_roomScrollbar.setScroll(0.f);
	}

	m_mode = mode;
}


void StateMain::showToolbar(float duration)
{
	if (GGame->getNavigationEnabled())
		m_navigation.show(duration);
	if (GGame->getMinimapEnabled() && m_mapRenderer.getMap())
		m_mapRenderer.show(duration * 4.f);
	TweenEngine::Tween::to(m_buttonInventory, Button::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonSettings, Button::ALPHA, duration)
		.target(180.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonTextLog, Button::ALPHA, duration)
		.target(180.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonInventory, Button::TEXTCOLOR_ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonSettings, Button::TEXTCOLOR_ALPHA, duration)
		.target(200.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonTextLog, Button::TEXTCOLOR_ALPHA, duration)
		.target(200.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_bgToolbar, TweenRectangleShape::FILL_COLOR_ALPHA, duration)
		.target(50.f)
		.start(m_tweenManager);
}

void StateMain::hideToolbar(float duration)
{
	m_navigation.hide(duration);
	m_mapRenderer.hide(duration);
	TweenEngine::Tween::to(m_buttonInventory, Button::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonSettings, Button::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonTextLog, Button::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonInventory, Button::TEXTCOLOR_ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonSettings, Button::TEXTCOLOR_ALPHA, duration)
		.target(60.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonTextLog, Button::TEXTCOLOR_ALPHA, duration)
		.target(60.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_bgToolbar, TweenRectangleShape::FILL_COLOR_ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
}

void StateMain::setScroll(float position)
{
//	std::cout << "StateMain::setScroll " << position << std::endl;
//	std::cout << "  scroll area: " << m_scrollAreaSize.x << " " << m_scrollAreaSize.y << std::endl;
	m_scrollPos = round(position);
	repositionText();
}

float StateMain::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &StateMain::getScrollSize()
{
	return m_scrollAreaSize;
}

int StateMain::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case ACTION_BUILDER:
		returnValues[0] = m_actionBuilderShowPos;
		return 1;
	default:
		return State::getValues(tweenType, returnValues);
	}
}

void StateMain::setValues(int tweenType, float *newValues)
{
	if (tweenType == ACTION_BUILDER)
		setActionBuilderShowPos(newValues[0]);
	else
		State::setValues(tweenType, newValues);
}

bool StateMain::processAction(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	for (auto &objectId : objectIds)
		if (!GGame->getRoom()->containsId(objectId) && !GGame->getObjectList()->containsId(objectId))
			return false;

	auto success = ScriptMan->runActionScript(verbId, objectIds, ProjData[ID::scriptBeforeAction].ToString());
	if (!success)
		return false;

	auto action = Action::find(getContext(), verbId, objectIds);
	if (action)
		success = action->runScript();
	else
		success = ScriptMan->runActionScript(verbId, objectIds);

	if (!success)
		return false;
	success = ScriptMan->runActionScript(verbId, objectIds, ProjData[ID::scriptAfterAction].ToString());

	if (success)
	{
		// Don't record during playback to avoid duplicating all actions
		if (m_testRecordMode && !m_testPlaybackMode)
		{
			auto jobjects = sj::Array();
			for (auto &objectId : objectIds)
				jobjects.append(objectId);
			json jtestItem({
				"type", "action",
				"verb", verbId,
				"objects", jobjects
			});
			runCallback(&jtestItem);
		}
		updateUI();
	}
	return success;
}

void StateMain::updateUI()
{
	m_mapRenderer.evaluateScripts();
	updateRoomText();
}

void StateMain::updateRoomText(const std::string &newText, float duration)
{
	auto room = GGame->getRoom();
	auto text = newText;
	auto firstVisit = !GSaveData[ID::roomDescriptions].hasKey(room->getId());
	if (text == " ")
		text = room->getDescription();
	if (text == m_roomActiveText.getText())
		return;

	m_roomTextChanging = true;
	m_roomActiveTextFadeOut = m_roomActiveText;
	m_roomActiveTextFadeOut.setHighlightId("");
	m_roomActiveText.setText(text);

	m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
	updateScrollbar();

	m_roomActiveText.setAlpha(0.f);
	TweenEngine::Tween::to(m_roomActiveText, ActiveText::ALPHA, duration)
		.target(255.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_roomActiveTextFadeOut, ActiveText::ALPHA, duration)
		.target(0.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
			m_roomTextChanging = false;
		}).start(m_tweenManager);

	m_tweenManagerHighlights.killAll();
	auto highlightDuration = firstVisit ? 3.f + 1.f / 100.f * text.size() : 0.f;
	auto highlightDelay = firstVisit ? 2.f : 0.f;
	m_roomActiveText.setHighlightFactor(0.f);
	m_navigation.setHighlightFactor(0.f);
	m_navigation.setPaths(room->getPaths());
	TweenEngine::Tween::to(m_roomActiveText, ActiveText::HIGHLIGHTS, highlightDuration)
		.target(1.f)
		.delay(highlightDelay)
		.start(m_tweenManagerHighlights);
	TweenEngine::Tween::to(m_navigation, Navigation::HIGHLIGHTS, highlightDuration)
		.target(1.f)
		.delay(highlightDelay)
		.start(m_tweenManagerHighlights);
}

void StateMain::setActionBuilderShowPos(float position)
{
	auto width = GConfig.width;
	auto height = GConfig.height;
	auto toolbarHeight = m_bgToolbar.getSize().y;
	auto pos = position * m_actionBuilder.getSize().y;

	m_actionBuilder.setPosition(m_actionBuilder.getPosition().x, height - toolbarHeight - pos);

	m_roomScrollbar.setSize(sf::Vector2f(2, height - toolbarHeight - pos - 8.f));
	m_roomScrollbar.setScrollAreaSize(sf::Vector2f(width, height - toolbarHeight - pos));
	m_roomScrollbar.setDragRect(sf::FloatRect(0.f, 0.f, width, height - toolbarHeight - pos));
	updateScrollbar();
	m_roomScrollbar.setScroll(m_scrollPos);

	m_roomTextView.reset(sf::FloatRect(0.f, 0.f, width, height - toolbarHeight - pos));
	m_roomTextView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, (height - toolbarHeight - pos) / height));

	m_actionBuilderShowPos = position;
}

void StateMain::callOverlayFunc()
{
	EventMan->trigger(StateEvent::MessageContinue);
		updateUI();
}

void StateMain::repositionText()
{
	auto w = GConfig.width;
	m_roomActiveText.setPosition((w - m_roomActiveText.getSize().x)/2, m_roomTextPadding + m_scrollPos);
}

void StateMain::quit()
{
	if (m_quitting)
		return;

	m_quitting = true;
	m_tweenManager.killAll();
	m_tweenManagerHighlights.killAll();

	auto duration = 1.f;
	TweenEngine::Tween::to(m_roomActiveText, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_roomActiveTextFadeOut, ActiveText::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_bgToolbar, TweenRectangleShape::FILL_COLOR_ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonInventory, Button::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonSettings, Button::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);
	TweenEngine::Tween::to(m_buttonTextLog, Button::ALPHA, duration)
		.target(0.f)
		.start(m_tweenManager);

	m_bg.setFillColor(sf::Color(200, 200, 200));
	TweenEngine::Tween::to(m_bg, TweenRectangleShape::FILL_COLOR_RGB, duration)
		.target(255.f, 255.f, 255.f)
		.start(m_tweenManager);

	m_dialogueRenderer.hide(duration);
	m_textOverlay.hide(duration);
	m_inventory.hide(duration);
	m_verbList.hide(duration);
	m_actionBuilder.hide(duration);
	m_navigation.hide(duration);
	m_mapRenderer.hide(duration);

	TweenEngine::Tween::mark()
		.delay(duration)
		.setCallback(TweenEngine::TweenCallback::BEGIN, [this](TweenEngine::BaseTween*){
			requestStackClear();
			requestStackPush(StateID::TitleScreen);
		})
		.start(m_tweenManager);
}

bool StateMain::processEvent(const sf::Event &event)
{
	if (m_quitting)
		return true;

	if (m_buttonSettings.processEvent(event) || m_buttonTextLog.processEvent(event))
		return true;

	if (m_textOverlay.isVisible())
	{
		if (m_textOverlay.processEvent(event))
			m_textOverlay.hide(0.5f, TextOverlay::ALPHA, [this](){
				callOverlayFunc();
			});
		if (m_mode == ModeRoom)
			showToolbar(0.5f);
		return true;
	}

	if (m_roomTextChanging)
		return true;

	if (m_mode == ModeCutscene)
	{
		if ((m_cutsceneRenderer.isComplete() || m_cutsceneRenderer.isWaitingForClick()) && m_cutsceneRenderer.processEvent(event))
			return true;
		if (event.type == sf::Event::MouseButtonReleased)
		{
			m_cutsceneRenderer.click();
			m_cutsceneSpeed = 1.f;
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
		}
	}
	else if (m_mode == ModeDialogue)
	{
		m_dialogueRenderer.processEvent(event);
	}
	else if (m_mode == ModeRoom)
	{
		if (m_verbList.processEvent(event)) {
			return false;
		} else {
			if (m_buttonInventory.processEvent(event))
				return true;
			// Returns true if an object is clicked on
			if (m_inventory.processEvent(event))
				return true;
		}
		if (GGame->getMinimapEnabled() && m_mapRenderer.getMap()) {
			if (!m_mapRenderer.processEvent(event))
				return false;
		}

		if (m_actionBuilder.processEvent(event))
			return true;
		if (GGame->getNavigationEnabled())
			m_navigation.processEvent(event);

		if (m_buttonInventory.processEvent(event))
			return true;
		if (m_roomScrollbar.processEvent(event))
		{
			if (event.type != sf::Event::MouseButtonPressed)
			{
				m_quickVerbPressed = false;
				return true;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
		{
			auto p = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
			auto word = m_roomActiveText.objectFromPoint(p);

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (!word.empty()) {
					m_quickVerbPressed = true;
					m_selectedObjectId = word;
					m_clock.restart();
				}
			}
			else if (m_quickVerbPressed)
			{
				m_quickVerbPressed = false;
				m_roomActiveText.setHighlightId(word);
				if (!word.empty())
				{
					m_selectedObjectId = word;
					if (!m_actionBuilder.isVisible())
					{
						m_verbList.setVerbs(word);
						m_verbList.setPositionBounded(p);
						m_verbList.show();
					} else {
						m_actionBuilder.setObject(m_selectedObjectId);
						m_actionBuilder.selectNextEmptyIndex();
					}
				} else if (m_verbList.isVisible())
					m_verbList.hide();
			}
		}
	}

	return true;
}

bool StateMain::update(float delta)
{
	if (GGame->isQuitting())
		quit();

	auto& map = GGame->getMap();
	auto& oldMap = m_mapRenderer.getMap();
	if (map && (!oldMap || oldMap->getId() != map->getId()))
		m_mapRenderer.setMap(map);


	m_dialogueRenderer.update(delta);
	if (m_mode == ModeRoom)
	{
		m_roomActiveText.update(delta);
		m_roomActiveTextFadeOut.update(delta);
		if (m_quickVerbPressed && m_clock.getElapsedTime() > sf::milliseconds(800))
		{
			auto callback = m_verbList.getSelectCallback();
			m_quickVerbPressed = false;
			if (callback)
				callback(ProjData[ID::quickVerb].ToString());
		}
		if (GGame->getNavigationEnabled() && !m_quitting)
			m_navigation.show();
		else
			m_navigation.hide();

		if (GGame->getMinimapEnabled() && m_mapRenderer.getMap() && !m_quitting)
			m_mapRenderer.show(2.f);
		else
			m_mapRenderer.hide();
	}

	if (m_mode == ModeCutscene)
	{
		m_cutsceneRenderer.update(delta * m_cutsceneSpeed);
		if (m_cutsceneRenderer.isComplete())
		{
			EventMan->push(StateEvent::CutsceneComplete);
		}
	}

	m_roomScrollbar.update(delta);
	m_verbList.update(delta);
	m_actionBuilder.update(delta);
	m_inventory.update(delta);
	m_mapRenderer.update(delta);
	m_navigation.update(delta);
	m_textOverlay.update(delta);
	m_iconSave.update(delta);


	m_tweenManager.update(delta);
	m_tweenManagerHighlights.update(delta);
	return true;
}

} // namespace NovelTea
