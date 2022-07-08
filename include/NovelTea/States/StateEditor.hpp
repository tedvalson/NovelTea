#ifndef NOVELTEA_STATEEDITOR_HPP
#define NOVELTEA_STATEEDITOR_HPP

#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/MapRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace NovelTea
{

using TestCallback = std::function<bool(const json&)>;

enum class StateEditorMode : int {
	Nothing  = 0,
	Cutscene = 1,
	Room,
	Dialogue,
	Map,
	Text,
};

class StateEditor : public State, public Scrollable
{
public:
	StateEditor(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void repositionText();

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	void *processData(void *data) override;

private:
	CutsceneRenderer m_cutsceneRenderer;
	MapRenderer m_mapRenderer;

	ActiveText m_previewText;
	TextProperties m_textProps;

	ActiveText m_roomActiveText;
	float m_scrollPos;
	float m_roomTextPadding;
	sf::Vector2f m_size;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_roomScrollbar;

	TweenEngine::TweenManager m_tweenManager;
	StateEditorMode m_mode;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEEDITOR_HPP
