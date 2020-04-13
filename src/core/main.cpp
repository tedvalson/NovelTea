#include <NovelTea/Engine.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char **argv)
{
	sf::RenderWindow window(sf::VideoMode(480/2, 700/2, 16), "NovelTea Launcher");
	NovelTea::EngineConfig config;
	config.width = 480;
	config.height = 700;
	config.initialState = NovelTea::StateID::Main;
	auto engine = new NovelTea::Engine(config);
	engine->initialize();
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			engine->processEvent(event);
		}
		
		window.clear(sf::Color::White);
		engine->update(0.05f);
		engine->render(window);
		window.display();
		
		//std::cout << "frame processed" << std::endl;
	}
	delete engine;
	return 0;
}
