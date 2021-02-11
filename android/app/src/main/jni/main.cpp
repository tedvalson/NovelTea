#include <NovelTea/Engine.hpp>
#include <NovelTea/SaveData.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char *argv[])
{
	sf::VideoMode screen(sf::VideoMode::getDesktopMode());

	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);

	std::string saveDir = getenv("EXTERNAL_STORAGE");
	saveDir += "/test";
	Proj.loadFromFile(saveDir + "/test.ntp");
	
	NovelTea::EngineConfig config;
	config.width = window.getSize().x;
	config.height = window.getSize().y;
	config.fps = 30;
	config.initialState = NovelTea::StateID::Intro;
	auto engine = new NovelTea::Engine(config);
	engine->initialize();

	if (!saveDir.empty())
		GSave.setDirectory(saveDir);

	// We shouldn't try drawing to the screen while in background
	// so we'll have to track that. You can do minor background
	// work, but keep battery life in mind.
	bool active = true;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.isOpen() && (active ? window.pollEvent(event) : window.waitEvent(event)))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				engine->resize(event.size.width, event.size.height);
			// On Android MouseLeft/MouseEntered are (for now) triggered,
			// whenever the app loses or gains focus.
			else if (event.type == sf::Event::MouseLeft)
				active = false;
			else if (event.type == sf::Event::MouseEntered)
				active = true;

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
	return EXIT_SUCCESS;
}
