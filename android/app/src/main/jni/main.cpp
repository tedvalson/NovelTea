#include <NovelTea/Engine.hpp>
#include <NovelTea/SaveData.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char *argv[])
{
	sf::VideoMode screen(sf::VideoMode::getDesktopMode());

	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);
	
	NovelTea::EngineConfig config;
	config.width = 480;
	config.height = 700;
	config.fps = 30;
	config.initialState = NovelTea::StateID::Main;
	auto engine = new NovelTea::Engine(config);
	engine->initialize();

	Proj.loadFromFile("test.ntp");
	Save.setDirectory(".");

	// We shouldn't try drawing to the screen while in background
	// so we'll have to track that. You can do minor background
	// work, but keep battery life in mind.
	bool active = true;

	while (window.isOpen())
	{
		sf::Event event;

		while (active ? window.pollEvent(event) : window.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				engine->resize(event.size.width, event.size.height);

			engine->processEvent(event);
		}
		
		if (active)
		{
			engine->update();
			engine->render(window);
			window.display();
		}
		else
			sf::sleep(sf::milliseconds(100));
	}
	
	delete engine;
	return 0;
}
