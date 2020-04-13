#ifndef NOVELTEA_STATEMAIN_H
#define NOVELTEA_STATEMAIN_H

#include <NovelTea/States/State.hpp>
#include <SFML/Graphics.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/RichText.hpp>
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
	virtual bool processEvent(const sf::Event &event);
	virtual bool update(float deltaSeconds);
	virtual void render(sf::RenderTarget &target);

	virtual void *processData(void *data);

private:
	sf::Font font;
	TweenText text;
	TweenRectangleShape shape;
	RichText richText;
	sf::Texture texture;
	TweenEngine::TweenManager tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEMAIN_H
