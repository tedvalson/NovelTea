#ifndef NOVELTEA_STATEEDITOR_HPP
#define NOVELTEA_STATEEDITOR_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <json.hpp>

using json = nlohmann::json;

namespace NovelTea
{

enum class StateEditorMode : int {
	Nothing  = 0,
	Cutscene = 1,
	Room,
};

class StateEditor : public State
{
public:
	StateEditor(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float deltaSeconds) override;
	void render(sf::RenderTarget &target) override;

	void *processData(void *data) override;

private:
	sf::Font font;
	TweenText text;
	CutsceneRenderer cutsceneRenderer;
	ActiveText activeText;
	TweenRectangleShape shape;
	sf::Texture texture;
	TweenEngine::TweenManager tweenManager;
	StateEditorMode mode;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEEDITOR_HPP
