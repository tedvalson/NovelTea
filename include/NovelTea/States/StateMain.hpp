#ifndef NOVELTEA_STATEMAIN_HPP
#define NOVELTEA_STATEMAIN_HPP

#include <SFML/Graphics.hpp>
#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/CutsceneRenderer.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/GUI/ActionBuilder.hpp>
#include <json.hpp>

using json = nlohmann::json;

namespace NovelTea
{

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

	void updateRoomText();

private:
	sf::Font font;
	TweenText text;
	Mode m_mode;

	ActiveText m_roomActiveText;
	VerbList m_verbList;
	ActionBuilder m_actionBuilder;
	std::string m_selectedObjectId;

	CutsceneRenderer m_cutsceneRenderer;
	float m_cutsceneSpeed;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_HPP
