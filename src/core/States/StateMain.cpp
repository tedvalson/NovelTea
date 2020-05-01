#include <NovelTea/States/StateMain.hpp>
#include <NovelTea/Engine.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

StateMain::StateMain(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	font.loadFromFile("/home/android/dev/NovelTea/res/fonts/DejaVuSans.ttf");

	text.setFont(font);
	text.setCharacterSize(30);
	text.setString("Testing test!?");
	text.setFillColor(sf::Color::Black);
	text.setOutlineColor(sf::Color::Yellow);
	text.setOutlineThickness(1.f);
	text.setPosition(0.f, 0.f);

//	text.setOrigin(text.getLocalBounds().getSize() / 2.f);

	shape.setSize(sf::Vector2f(400.f, 300.f) / 2.f);
	shape.setPosition(5.f, 10.f);
	shape.setTexture(&texture);
	shape.setOutlineColor(sf::Color::Red);



//	TweenEngine::Tween::to(text, TweenText::OUTLINE_THICKNESS, 0.5f)
//		.target(5.f)
//		.repeatYoyo(-1, 0.f)
//		.start(tweenManager);

}

void StateMain::render(sf::RenderTarget &target)
{
//	target.draw(shape);
	target.draw(text);
}

void *StateMain::processData(void *data)
{
	auto &jsonData = *static_cast<json*>(data);
	auto resp = new json;
	auto event = jsonData["event"];

	if (jsonData["type"] == "test")
		text.setString(jsonData.value("str", "def"));

	if (event == "load")
	{

	}
	else if (event == "update")
	{
		json d = jsonData["data"];
		std::cout << d.dump() << std::endl;
	}

//	(*resp)["test"] = "pass";
	resp->at("test") = "pass";

	return resp;
}

bool StateMain::processEvent(const sf::Event &event)
{
	if (event.type == sf::Event::TouchEnded || event.type == sf::Event::MouseButtonReleased)
	{
		int x = (event.type == sf::Event::TouchEnded) ? event.touch.x: event.mouseButton.x;
		int y = (event.type == sf::Event::TouchEnded) ? event.touch.y: event.mouseButton.y;

		TweenEngine::Tween::to(text, TweenText::POSITION_XY, 5.f)
				.target(x, y)
				.start(tweenManager);

		TweenEngine::Tween::to(shape, TweenRectangleShape::OUTLINE_THICKNESS, 1.f)
				.target(x / 2)
				.start(tweenManager);
	}

	//std::cout << event.type << std::endl;

	return true;
}

bool StateMain::update(float deltaSeconds)
{
//	std::cout << "StateMain::update " << deltaSeconds << std::endl;
	tweenManager.update(deltaSeconds);
	return true;
}

} // namespace NovelTea
