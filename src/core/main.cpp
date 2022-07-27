#include <NovelTea/Engine.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/TextInput.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

sf::RenderWindow* window;

void triggerTextInput(const std::string &message, int ref)
{
	std::string input;
	std::cout << message << std::endl;
	std::cin >> input;
	GTextInput.callback(input, ref);
	sf::Event event;
	while (window->pollEvent(event)) {}
}

int main(int argc, char **argv)
{
	NovelTea::AssetPath::set("/home/android/dev/NovelTea/res/assets/");
	
	auto dir = "/home/android/dev/NovelTea/bin";
	GSettings.setDirectory(dir);
	GSettings.load();
	GTextInput.textInputTrigger = triggerTextInput;

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
	engine->setFramerateLocked(false);
	
	// NovelTeaLaunch project.ntp entity typeId nameId [segmentId]
	if (argc >= 5) {
		auto command = std::string{argv[2]};
		if (command == "entity") {
			auto typeId = atoi(argv[3]);
			auto idName = argv[4];
			Proj.loadFromFile(argv[1]);
			GSave->data()[NovelTea::ID::entrypointEntity] = sj::Array(typeId, idName);
			GSave->data()[NovelTea::ID::entityPreview] = true;
			if (argc == 6) {
				auto segmentId = atoi(argv[5]);
				GSave->data()[NovelTea::ID::entrypointMetadata] = sj::Array("", sj::Array(segmentId));
			}
			// Run the "after load" script even though no real game load happened
			ActiveGame->getScriptManager()->runInClosure(ProjData[NovelTea::ID::scriptAfterLoad].ToString());
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
	
	window = new sf::RenderWindow(sf::VideoMode(480/2, 700/2, 16), "NovelTea Launcher");
	
	GSave->setDirectory(dir);
	
	auto active = true;
	while (window->isOpen())
	{
		sf::Event event;
		while (active ? window->pollEvent(event) : window->waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
			else if (event.type == sf::Event::Resized)
				engine->resize(event.size.width, event.size.height);
			else if (event.type == sf::Event::LostFocus)
				active = false;
			else if (event.type == sf::Event::GainedFocus) {
				engine->update(0.f);
				active = true;
			}

			engine->processEvent(event);
		}
		
		engine->update();
		engine->render(*window);
		window->display();
		
		//std::cout << "frame processed" << std::endl;
	}
	delete engine;
	delete window;
	return 0;
}
