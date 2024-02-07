#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <NovelTea/SFML/Utils.hpp>
#include <NovelTea/SFML/AssetLoaderSFML.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateEditor::StateEditor(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
, m_cutsceneRenderer(&context)
, m_mapRenderer(&context)
, m_previewText(&context)
, m_shader(nullptr)
, m_roomActiveText(&context)
, m_scrollPos(0.f)
, m_mode(StateEditorMode::Nothing)
{

	m_cutsceneRenderer.setSkipWaitingForClick(true);
	m_cutsceneRenderer.setSkipScriptSegments(true);
	m_cutsceneRenderer.setSkipConditionChecks(true);

	m_roomScrollbar.setColor(sf::Color(0, 0, 0, 40));
	m_roomScrollbar.setAutoHide(false);
	m_roomScrollbar.attachObject(this);

	m_mapRenderer.setShowEverything(true);
	m_mapRenderer.setMiniMapMode(false, 0.f);
	m_mapRenderer.setModeLocked(true);

	m_textProps.color = fromColorSFML(sf::Color::Black);
	m_previewText.setSkipWaitingForClick(true);

	m_shaderText.setString("Text");
	m_shaderText.setFillColor(sf::Color::Black);
	m_shaderText.setFont(*Asset->font());

	m_eventListenerId = EventMan->listen([this](const EventPtr &event){
		auto type = event->type();
		if (type == ModeChanged) {
			m_mode = static_cast<StateEditorMode>(event->intVal);
			if (m_mode == StateEditorMode::Test) {
				auto callback = *reinterpret_cast<TestCallback*>(event->ptr);
				StateCallback stateCallback = [callback](void* data){
					auto& j = *reinterpret_cast<json*>(data);
					return callback(j);
				};
				requestStackClear();
				requestStackPush(StateID::Main, false, stateCallback);
			}
		}
		else if (type == EntityChanged) {
			if (m_mode == StateEditorMode::Cutscene) {
				auto cutscene = std::make_shared<Cutscene>(getContext());
				cutscene->fromJson(event->json);
				m_cutsceneRenderer.setCutscene(cutscene);
			}
			else if (m_mode == StateEditorMode::Map) {
				auto map = std::make_shared<Map>(getContext());
				map->fromJson(event->json);
				m_mapRenderer.setMap(map);
			}
			else if (m_mode == StateEditorMode::Room) {
				try {
					auto& jsonData = event->json;
					auto room = std::make_shared<Room>(getContext());
					room->fromJson(jsonData["room"]);

					GSaveData[NovelTea::ID::properties][NovelTea::Room::id][room->getId()] = jsonData["props"];
					GGame->setRoomId(room->getId());

					ScriptMan.reset();
					auto r = room->getDescription();
					m_roomActiveText.setText(r);
				} catch (std::exception &e) {
					m_roomActiveText.setText(e.what());
				}

				m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
				updateScrollbar();
				m_roomScrollbar.setScroll(m_scrollPos);
			}
			else if (m_mode == StateEditorMode::Shader) {
				m_startTime = Engine::getSystemTimeMs();
				m_shader = static_cast<sf::Shader*>(event->ptr);
				m_shader->setUniform("resolution", sf::Glsl::Vec2(m_size.x, m_size.y));
			}
		}
		else if (type == CutsceneSeek) {
			m_cutsceneRenderer.reset();
			m_cutsceneRenderer.update(0.001f * event->intVal);
		}
		else if (type == CutsceneUpdate) {
			m_cutsceneRenderer.update(0.001f * event->intVal);
		}
		else if (type == PreviewTextChanged) {
			m_previewText.setText(event->text, m_textProps);
		}
		else if (type == PreviewFontChanged) {
			m_textProps.fontAlias = event->text;
			m_previewText.updateProps(m_textProps);
			m_previewText.reset();
		}
		return true;
	});
}

StateEditor::~StateEditor()
{
	EventMan->remove(m_eventListenerId);
}

void StateEditor::render(sf::RenderTarget &target)
{
	if (m_mode == StateEditorMode::Cutscene)
		target.draw(m_cutsceneRenderer);
	else if (m_mode == StateEditorMode::Map)
		target.draw(m_mapRenderer);
	else if (m_mode == StateEditorMode::Room)
	{
		target.draw(m_roomActiveText);
		target.draw(m_roomScrollbar);
	}
	else if (m_mode == StateEditorMode::Text)
		target.draw(m_previewText);
	else if (m_mode == StateEditorMode::Shader)
	{
		if (m_shader)
			target.draw(m_shaderText, m_shader);
	}
}

void StateEditor::resize(const sf::Vector2f &size)
{
	m_size = size;

	if (m_shader) {
		m_shader->setUniform("resolution", sf::Glsl::Vec2(size.x, size.y));
	}

	auto fontSizeMultiplier = GConfig.fontSizeMultiplier;
	m_roomTextPadding = round(1.f / 16.f * std::min(size.x, size.y));

	m_roomActiveText.setSize(sf::Vector2f((size.x < size.y ? 1.f : 0.6f) * size.x - m_roomTextPadding*2, 0.f));
	m_roomActiveText.setFontSizeMultiplier(fontSizeMultiplier);

	m_textProps.fontSize = size.y / 4;
	m_previewText.setSize(size);
	m_previewText.setFontSizeMultiplier(fontSizeMultiplier);
	m_previewText.updateProps(m_textProps);

	m_shaderText.setCharacterSize(size.y / 4);
	m_shaderText.setOrigin(m_shaderText.getLocalBounds().width / 2, m_shaderText.getLocalBounds().height / 2);
	m_shaderText.setPosition(size.x / 2, size.y / 2);

	m_cutsceneRenderer.setMargin(m_roomTextPadding);
	m_cutsceneRenderer.setFontSizeMultiplier(fontSizeMultiplier);
	m_cutsceneRenderer.setSize(size);

	m_mapRenderer.setSize(size);

	m_roomScrollbar.setPosition(size.x - 4.f, 4.f);
	m_roomScrollbar.setSize(sf::Vector2f(2, size.y - 8.f));
	m_roomScrollbar.setScrollAreaSize(size);
	m_roomScrollbar.setDragRect(sf::FloatRect(0.f, 0.f, size.x, size.y));

	m_scrollAreaSize.y = m_roomTextPadding*2 + m_roomActiveText.getLocalBounds().height;
	updateScrollbar();
	m_roomScrollbar.setScroll(0.f);
}

void StateEditor::repositionText()
{
	auto w = GConfig.width;
	m_roomActiveText.setPosition((w - m_roomActiveText.getSize().x)/2, m_roomTextPadding + m_scrollPos);
}

void StateEditor::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionText();
}

float StateEditor::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &StateEditor::getScrollSize()
{
	return m_scrollAreaSize;
}

bool StateEditor::processEvent(const sf::Event &event)
{
	if (m_mode == StateEditorMode::Room)
	{
		if (m_roomScrollbar.processEvent(event))
			return true;
	}
	else if (m_mode == StateEditorMode::Map)
	{
		return m_mapRenderer.processEvent(event);
	}

	return true;
}

bool StateEditor::update(float delta)
{
	if (m_mode == StateEditorMode::Map)
		m_mapRenderer.update(delta);
	else if (m_mode == StateEditorMode::Shader && m_shader) {
		m_shader->setUniform("time", 0.001f * (Engine::getSystemTimeMs() - m_startTime));
	}
	if (!m_previewText.isAnimating())
		m_previewText.reset();
	m_previewText.update(delta);
	m_roomScrollbar.update(delta);
	m_tweenManager.update(delta);
	return true;
}

} // namespace NovelTea
