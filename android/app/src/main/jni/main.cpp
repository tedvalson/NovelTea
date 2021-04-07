#include <NovelTea/Engine.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>
#include <SFML/System/NativeActivity.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <android/native_activity.h>

int main(int argc, char *argv[])
{
	auto nativeActivity = sf::getNativeActivity();
	GSave.setDirectory(nativeActivity->internalDataPath);
	GSettings.setDirectory(nativeActivity->internalDataPath);
	GSettings.load();

	sf::VideoMode screen(sf::VideoMode::getDesktopMode());

	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);
	
	NovelTea::EngineConfig config;
	config.width = window.getSize().x;
	config.height = window.getSize().y;
	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.fps = 30;
	config.initialState = NovelTea::StateID::Intro;
	auto engine = new NovelTea::Engine(config);
	engine->initialize();

	std::string projDir = getenv("EXTERNAL_STORAGE");
	projDir += "/test";
	Proj.loadFromFile(projDir + "/test.ntp");
	

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
