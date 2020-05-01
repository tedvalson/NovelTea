#ifndef NOVELTEA_STATEMAIN_H
#define NOVELTEA_STATEMAIN_H

#include <NovelTea/States/State.hpp>
#include <SFML/Graphics.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <json.hpp>

using json = nlohmann::json;

namespace NovelTea
{

struct WidgetData
{
	nlohmann::json json;
};

struct WidgetRespData
{

};

class StateMain : public State
{
public:
	StateMain(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float deltaSeconds) override;
	void render(sf::RenderTarget &target) override;

	void *processData(void *data) override;

private:
	sf::Font font;
	TweenText text;
	TweenRectangleShape shape;
	sf::Texture texture;
	TweenEngine::TweenManager tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_H
