#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char **argv)
{
	auto dir = "/home/android/dev/NovelTea/bin";
	GSettings.setDirectory(dir);
	GSettings.load();

	sf::RenderWindow window(sf::VideoMode(480/2, 700/2, 16), "NovelTea Launcher");
	NovelTea::EngineConfig config;
	config.width = 480;
	config.height = 700;
	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.fps = 30;
	config.initialState = NovelTea::StateID::Intro;
	auto engine = new NovelTea::Engine(config);
	engine->initialize();
	
	GSave->setDirectory(dir);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				engine->resize(event.size.width, event.size.height);

			engine->processEvent(event);
		}
		
		engine->update();
		engine->render(window);
		window.display();
		
		//std::cout << "frame processed" << std::endl;
	}
	delete engine;
	return 0;
}
