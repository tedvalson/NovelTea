#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateMain::StateMain(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_mode(Mode::Nothing)
, m_testPlaybackMode(false)
, m_testRecordMode(false)
, m_roomTextChanging(false)
, m_scrollPos(0.f)
, m_cutsceneSpeed(1.f)
{
	ScriptMan.reset();

	auto width = getContext().config.width;
	auto height = getContext().config.height;
	auto toolbarHeight = round(height * 2.f/9.f);
	auto toolbarPadding = toolbarHeight / 10.f;
	m_roomTextPadding = round(1.f / 16.f * width);
	m_roomActiveText.setSize(sf::Vector2f(width - m_roomTextPadding*2, 0.f));
	m_cutsceneRenderer.setMargin(m_roomTextPadding);
	m_cutsceneRenderer.setSize(sf::Vector2f(width, height));
	m_inventory.setSize(sf::Vector2f(width, height));

	m_bgToolbar.setSize(sf::Vector2f(width, toolbarHeight));
	m_bgToolbar.setPosition(0.f, height - toolbarHeight);
	m_bgToolbar.setFillColor(sf::Color(0, 0, 0, 0));

	// Room
	m_roomScrollbar.setPosition(width - 4.f, 4.f);
	m_roomScrollbar.setColor(sf::Color(0, 0, 0, 40));
	m_roomScrollbar.setAutoHide(false);
	m_roomScrollbar.attachObject(this);

	// Cutscene
	m_cutsceneScrollbar.setPosition(width - 4.f, 4.f);
	m_cutsceneScrollbar.setColor(sf::Color(0, 0, 0, 40));
	m_cutsceneScrollbar.setAutoHide(false);
	m_cutsceneScrollbar.attachObject(&m_cutsceneRenderer);
	m_cutsceneScrollbar.setSize(sf::Vector2u(2, height - 8.f));
	m_cutsceneScrollbar.setScrollAreaSize(sf::Vector2u(width, height - m_roomTextPadding*2));

	// Navigation setup
	// Set all Navigation transforms before getGlobalBounds is called
	m_navigation.setSize(sf::Vector2f(toolbarHeight - toolbarPadding*2, toolbarHeight - toolbarPadding*2));
	m_navigation.setPosition(toolbarPadding, toolbarPadding + height - toolbarHeight);
	m_navigation.setCallback([this](int direction, const json &jentity){
		if (m_testRecordMode) {
			json jtestItem({
				"type", "move",
				"direction", direction
			});
			runCallback(&jtestItem);
		}
		GGame.pushNextEntityJson(jentity);
	});

	// Inventory setup
	m_inventory.setCallback([this](const std::string &objectId, float posX, float posY){
		if (m_actionBuilder.isVisible()) {
			m_actionBuilder.setObject(objectId);
			m_actionBuilder.selectNextEmptyIndex();
		} else {
			m_selectedObjectId = objectId;
			m_verbList.setVerbs(objectId);
			auto p = sf::Vector2f(posX - m_verbList.getLocalBounds().width, posY - m_verbList.getLocalBounds().height);
			m_verbList.setPositionBounded(p, sf::FloatRect(0.f, 0.f, getContext().config.width, getContext().config.height));
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
	});
	m_verbList.setShowHideCallback([this](bool showing){
		if (!showing) {
			m_roomActiveText.setHighlightId("");
			if (!m_selectedObjectId.empty() && m_actionBuilder.getObjects().size() > 1) {
				TweenEngine::Tween::to(*this, ACTION_BUILDER, 0.4f)
					.target(1.f)
					.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
						m_actionBuilder.show();
					})
					.start(m_tweenManager);
			}
		}
	});

	// ActionBuilder setup
	m_actionBuilder.setSize(sf::Vector2f(width - 20.f, 1.f / 4.f * height));
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

	// Notification setup
	Notification::setScreenSize(sf::Vector2f(width, m_actionBuilder.getPosition().y));

	// Dialogue setup
	m_dialogueRenderer.hide(0.f);
	m_dialogueRenderer.setSize(sf::Vector2f(width, height));

	// TextOverlay setup
	m_textOverlay.hide(0.f);
	m_textOverlay.setSize(sf::Vector2f(width, height - toolbarHeight));
	GGame.setMessageCallback([this](const std::vector<std::string> &messageArray, const DukValue &callback){
		m_textOverlayFunc = callback;
		if (!m_testPlaybackMode)
		{
			m_textOverlay.setTextArray(messageArray);
			m_textOverlay.show();
		}
		else
			callOverlayFunc();
	});

	auto &saveEntryPoint = GSave.data()[ID::entrypointEntity];
	auto &projEntryPoint = ProjData[ID::entrypointEntity];
	if (!saveEntryPoint.IsEmpty())
		setMode(saveEntryPoint);
	else if (!projEntryPoint.IsEmpty())
		setMode(projEntryPoint);

	processTestSteps();
}

void StateMain::render(sf::RenderTarget &target)
{
	if (m_mode != Mode::Room && m_roomTextChanging)
		target.draw(m_roomActiveTextFadeOut);

	if (m_mode == Mode::Cutscene)
	{
		target.draw(m_cutsceneRenderer);
		target.draw(m_cutsceneScrollbar);
	}
	else if (m_mode == Mode::Dialogue)
	{
	}
	else if (m_mode == Mode::Room)
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
	target.draw(m_navigation);
	target.draw(m_textOverlay);
	target.draw(m_inventory);
	if (m_verbList.isVisible())
		target.draw(m_verbList);

	for (auto &notification : Notification::notifications)
		target.draw(*notification);
}

void StateMain::setMode(Mode mode, const std::string &idName)
{
	m_roomActiveText.setHighlightId("");
	m_actionBuilder.hide(0.4f, ActionBuilder::ALPHA, [this](){
		TweenEngine::Tween::to(*this, ACTION_BUILDER, 0.4f)
			.target(0.f)
			.start(m_tweenManager);
	});
	m_verbList.hide();

	if (mode != Mode::Room)
	{
		updateRoomText("");
		hideToolbar();
	}
	if (mode != Mode::Dialogue)
		m_dialogueRenderer.hide();

	if (mode == Mode::Cutscene)
	{
		m_cutscene = GSave.get<Cutscene>(idName);
		m_cutsceneRenderer.setCutscene(m_cutscene);
	}
	else if (mode == Mode::Dialogue)
	{
		m_dialogue = GSave.get<Dialogue>(idName);
		m_dialogueRenderer.setDialogue(m_dialogue);
		m_dialogueRenderer.show();
	}
	else if (mode == Mode::Room)
	{
		auto nextRoom = GSave.get<Room>(idName);
		auto room = GGame.getRoom();
		if (room->getId() != idName)
		{
			if (!room->runScriptBeforeLeave() || !nextRoom->runScriptBeforeEnter()) {
				updateRoomText();
				return;
			}
			room->runScriptAfterLeave();
			GGame.setRoom(nextRoom);
			nextRoom->runScriptAfterEnter();
			m_navigation.setPaths(nextRoom->getPaths());
		}
		showToolbar();
		updateRoomText();
		m_roomScrollbar.setScroll(0.f);
	}

	m_mode = mode;
}

void StateMain::setMode(const json &jEntity)
{
	auto mode = Mode::Nothing;
	auto type = static_cast<EntityType>(jEntity[ID::selectEntityType].ToInt());
	auto idName = jEntity[ID::selectEntityId].ToString();

	if (type == EntityType::Cutscene)
		mode = Mode::Cutscene;
	else if (type == EntityType::Room)
		mode = Mode::Room;
	else if (type == EntityType::Dialogue)
		mode = Mode::Dialogue;
	else if (type == EntityType::Script) {
		ScriptMan.run(idName);
		return;
	}
	else if (type == EntityType::CustomScript) {
		ScriptMan.runInClosure(idName);
		return;
	}

	setMode(mode, idName);
}

void StateMain::showToolbar()
{
	m_navigation.show(1.f);
	m_inventory.show(1.f);
	TweenEngine::Tween::to(m_bgToolbar, TweenRectangleShape::FILL_COLOR_ALPHA, 1.f)
		.target(50.f)
		.start(m_tweenManager);
}

void StateMain::hideToolbar()
{
	m_navigation.hide();
	m_inventory.hide();
	TweenEngine::Tween::to(m_bgToolbar, TweenRectangleShape::FILL_COLOR_ALPHA, 1.f)
		.target(0.f)
		.start(m_tweenManager);
}

void StateMain::setScroll(float position)
{
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
		return -1;
	}
}

void StateMain::setValues(int tweenType, float *newValues)
{
	if (tweenType == ACTION_BUILDER)
		setActionBuilderShowPos(newValues[0]);
}

void StateMain::processTestSteps()
{
	if (!getContext().data.hasKey("testSteps"))
		return;

	m_testPlaybackMode = true;
	m_testRecordMode = getContext().data["record"].ToBool();
	if (m_testRecordMode)
		m_dialogueRenderer.setDialogueCallback([this](int index){
			json jtestItem({
				"type", "dialogue",
				"index", index
			});
			if (!m_testPlaybackMode)
				runCallback(&jtestItem);
		});

	auto success = true;
	auto jsteps = getContext().data["testSteps"];
	m_cutsceneRenderer.setSkipWaitingForClick(true);
	for (int i = 0; i < jsteps.size(); ++i)
	{
		auto &jstep = jsteps[i];
		auto type = jstep["type"].ToString();

		auto waiting = (type == "wait");
		auto waitTimeLeft = 0.f;
		if (waiting)
			waitTimeLeft = 0.001f * jstep["duration"].ToInt();

		do {
			if (m_mode == Mode::Cutscene) {
				m_cutsceneRenderer.update(0.001f * m_cutscene->getDelayMs());
				if (m_cutsceneRenderer.isComplete())
					GGame.pushNextEntityJson(m_cutscene->getNextEntity());
			}
			else if (m_mode == Mode::Dialogue) {
				m_dialogueRenderer.processLines();
				if (m_dialogueRenderer.isComplete())
					GGame.pushNextEntityJson(m_dialogue->getNextEntity());
				else
					break;
			}

			if (waiting && waitTimeLeft > 0.f) {
				if (GGame.getTimerManager().update(0.01f))
					updateRoomText();
				waitTimeLeft -= 0.01f;
			}
		}
		while (gotoNextEntity() || (waitTimeLeft > 0.f));


		if (type == "action")
		{
			std::vector<std::string> objectIds;
			for (auto jobjectId : jstep["objects"].ArrayRange())
				objectIds.push_back(jobjectId.ToString());
			if (!processAction(jstep["verb"].ToString(), objectIds))
				success = false;
		}
		else if (type == "dialogue")
		{
			if (!m_dialogueRenderer.processSelection(jstep["index"].ToInt()))
				success = false;
		}
		else if (type == "move")
		{
			auto direction = jstep["direction"].ToInt();
			auto &paths = m_navigation.getPaths();
			auto &jentity = paths[direction][1];
			success = (paths[direction][0].ToBool() && jentity[0].ToInt() != -1);
			if (success)
				GGame.pushNextEntityJson(jentity);
		}

		if (!success)
		{
			json j({
				"success", false,
				"index", i
			});
			runCallback(&j);
			std::cout << "FAILED" << std::endl;
			break;
		}
	}

	m_cutsceneRenderer.setSkipWaitingForClick(false);
	m_testPlaybackMode = false;
}

bool StateMain::processAction(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	auto action = Action::find(verbId, objectIds);
	auto verb = GSave.get<Verb>(verbId);

	for (auto &objectId : objectIds)
		if (!GGame.getRoom()->containsId(objectId) && !GGame.getObjectList()->containsId(objectId))
			return false;

	auto success = ScriptMan.runActionScript(verbId, objectIds, ProjData[ID::scriptBeforeAction].ToString());
	if (!success)
		return false;

	if (action)
		success = action->runScript();
	else if (!verb->getScriptDefault().empty())
		success = ScriptMan.runActionScript(verbId, objectIds, verb->getScriptDefault());
	else
		success = ScriptMan.runActionScript(verbId, objectIds, ProjData[ID::scriptUndefinedAction].ToString());

	if (!success)
		return false;
	success = ScriptMan.runActionScript(verbId, objectIds, ProjData[ID::scriptAfterAction].ToString());

	if (success)
	{
		// Don't record during playback to avoid dupliating all actions
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
		updateRoomText();
	}
	return success;
}

bool StateMain::gotoNextEntity()
{
	auto nextEntity = GGame.popNextEntity();
	if (!nextEntity)
	{
		if (m_mode != Mode::Room)
			nextEntity = GGame.getRoom();
		if (!nextEntity || nextEntity->getId().empty())
			return false;
	}

	auto mode = Mode::Nothing;
	if (nextEntity->entityId() == Cutscene::id)
		mode = Mode::Cutscene;
	else if (nextEntity->entityId() == Room::id)
		mode = Mode::Room;
	else if (nextEntity->entityId() == Dialogue::id)
		mode = Mode::Dialogue;
	else if (nextEntity->entityId() == Script::id) {
		auto script = std::static_pointer_cast<Script>(nextEntity);
		ScriptMan.runScript(script);
		return true;
	}
	setMode(mode, nextEntity->getId());
	return true;
}

void StateMain::updateRoomText(const std::string &newText, float duration)
{
	auto room = GGame.getRoom();
	auto text = newText;
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
}

void StateMain::setActionBuilderShowPos(float position)
{
	auto width = getContext().config.width;
	auto height = getContext().config.height;
	auto toolbarHeight = m_bgToolbar.getSize().y;
	auto pos = position * m_actionBuilder.getSize().y;

	m_actionBuilder.setPosition(10.f, height - toolbarHeight - pos);

	m_roomScrollbar.setSize(sf::Vector2u(2, height - toolbarHeight - pos - 8.f));
	m_roomScrollbar.setScrollAreaSize(sf::Vector2u(width, height - toolbarHeight - pos));
	m_roomScrollbar.setDragRect(sf::FloatRect(0.f, 0.f, width, height - toolbarHeight - pos));
	updateScrollbar();
	m_roomScrollbar.setScroll(m_scrollPos);

	m_roomTextView.reset(sf::FloatRect(0.f, 0.f, width, height - toolbarHeight - pos));
	m_roomTextView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, (height - toolbarHeight - pos) / height));

	m_actionBuilderShowPos = position;
}

void StateMain::callOverlayFunc()
{
	if (m_textOverlayFunc.type() != DukValue::UNDEFINED){
		ScriptMan.call<void>(m_textOverlayFunc);
		updateRoomText();
	}
}

void StateMain::repositionText()
{
	m_roomActiveText.setPosition(m_roomTextPadding, m_roomTextPadding + m_scrollPos);
}

bool StateMain::processEvent(const sf::Event &event)
{
	if (m_textOverlay.isVisible())
	{
		if (m_textOverlay.processEvent(event))
			m_textOverlay.hide(0.5f, TextOverlay::ALPHA, [this](){
				callOverlayFunc();
			});
		return true;
	}

	if (m_roomTextChanging)
		return true;

	if (m_mode == Mode::Cutscene)
	{
		if ((m_cutsceneRenderer.isComplete() || m_cutsceneRenderer.isWaitingForClick()) && m_cutsceneScrollbar.processEvent(event))
			return true;
		if (event.type == sf::Event::MouseButtonReleased)
		{
			m_cutsceneRenderer.click();
			m_cutsceneSpeed = 1.f;
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			m_cutsceneSpeed = 10.f;
		}
	}
	else if (m_mode == Mode::Dialogue)
	{
		m_dialogueRenderer.processEvent(event);
	}
	else if (m_mode == Mode::Room)
	{
		if (m_verbList.isVisible() && m_verbList.processEvent(event)) {
			return false;
		} else {
			// Returns true if an object is clicked on
			if (m_inventory.processEvent(event))
				return true;
		}

		if (m_actionBuilder.isVisible())
			m_actionBuilder.processEvent(event);
		m_navigation.processEvent(event);

		if (m_roomScrollbar.processEvent(event))
			return true;

		if (event.type == sf::Event::MouseButtonReleased)
		{
			{
				auto p = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
				auto word = m_roomActiveText.objectFromPoint(p);
				m_roomActiveText.setHighlightId(word);
				if (!word.empty())
				{
					m_selectedObjectId = word;
					if (!m_actionBuilder.isVisible())
					{
						m_verbList.setVerbs(word);
						m_verbList.setPositionBounded(p, sf::FloatRect(0.f, 0.f, getContext().config.width, getContext().config.height));
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
	m_dialogueRenderer.update(delta);

	if (m_mode == Mode::Cutscene)
	{
		m_cutsceneScrollbar.update(delta);
		m_cutsceneRenderer.update(delta * m_cutsceneSpeed);
		if (m_cutsceneRenderer.isComplete())
		{
			GGame.pushNextEntityJson(m_cutscene->getNextEntity());
			gotoNextEntity();
		}
	}
	else if (m_mode == Mode::Dialogue)
	{
		if (m_dialogueRenderer.isComplete())
		{
			GGame.pushNextEntityJson(m_dialogue->getNextEntity());
			gotoNextEntity();
		}
	}
	else // if Mode::Nothing
		if (!m_roomTextChanging)
			gotoNextEntity();

	m_roomScrollbar.update(delta);
	m_verbList.update(delta);
	m_actionBuilder.update(delta);
	m_inventory.update(delta);
	m_navigation.update(delta);
	m_textOverlay.update(delta);

	Notification::update(delta);
	if (GGame.getTimerManager().update(delta))
		updateRoomText();

	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
