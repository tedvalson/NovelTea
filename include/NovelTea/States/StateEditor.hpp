#ifndef NOVELTEA_STATEEDITOR_HPP
#define NOVELTEA_STATEEDITOR_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace NovelTea
{

enum class StateEditorMode : int {
	Nothing  = 0,
	Cutscene = 1,
	Room,
	Dialogue,
};

class StateEditor : public State
{
public:
	StateEditor(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

	void *processData(void *data) override;

private:
	TweenText text;
	TweenRectangleShape shape;
	sf::Texture texture;

	CutsceneRenderer m_cutsceneRenderer;
	ActiveText m_roomActiveText;

	VerbList m_verbList;
	ActionBuilder m_actionBuilder;

	std::string m_selectedObjectId;

	TweenEngine::TweenManager m_tweenManager;
	StateEditorMode m_mode;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEEDITOR_HPP
