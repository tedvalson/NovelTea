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

	NovelTea::EngineConfig config;
	config.width = 480;
	config.height = 700;
	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.saveDir = dir;
	if (argc >= 5)
		config.initialState = NovelTea::StateID::Main;
	else
		config.initialState = NovelTea::StateID::TitleScreen;
	
	auto engine = new NovelTea::Engine(config);
	engine->initialize();
	
	// NovelTeaLaunch project.ntp entity typeId nameId [segmentId]
	if (argc >= 5) {
		auto command = std::string{argv[2]};
		if (command == "entity") {
			auto typeId = atoi(argv[3]);
			auto idName = argv[4];
			Proj.loadFromFile(argv[1]);
			GSave->data()[NovelTea::ID::entrypointEntity] = sj::Array(typeId, idName);
			if (argc == 6) {
				auto segmentId = atoi(argv[5]);
				GSave->data()[NovelTea::ID::entrypointMetadata] = sj::Array("", sj::Array(segmentId));
			}
		} else {
			std::cout << "Unknown command: " << argv[2] << std::endl;
			return 1;
		}
	} else if (argc == 2) {
		std::string fileName = argv[1];
		Proj.loadFromFile(fileName);
	} else if (argc == 1) {
	} else {
		return 1;
	}
	
	sf::RenderWindow window(sf::VideoMode(480/2, 700/2, 16), "NovelTea Launcher");
	
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
