#include <NovelTea/Engine.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/AssetManager.hpp>
#include <iostream>

int main(int argc, char **argv)
{
	NovelTea::AssetPath::set("/home/android/dev/NovelTea/res/assets/");
	auto dir = "/home/android/dev/NovelTea/bin";
	
	NovelTea::ContextConfig config;
	config.width = 480/2;
	config.height = 700/2;
//	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
//	config.dpiMultiplier = 2.f;
	config.saveDir = dir;
	config.settingsDir = dir;
//	config.entityPreview = false;
	if (argc >= 5)
		config.initialState = NovelTea::StateID::Main;
	else
		config.initialState = NovelTea::StateID::Intro;

	// NovelTeaLaunch project.ntp entity typeId nameId [segmentId]
	if (argc >= 5) {
		auto command = std::string{argv[2]};
		if (command == "entity") {
			auto typeId = atoi(argv[3]);
			auto idName = argv[4];
			config.projectFileName = argv[1];
			config.entityPreview = true;
			config.entryEntity = sj::Array(typeId, idName);
			if (argc == 6) {
				auto segmentId = atoi(argv[5]);
				config.entryMeta = sj::Array("", sj::Array(segmentId));
			}
		} else {
			std::cout << "Unknown command: " << argv[2] << std::endl;
			return 1;
		}
	} else if (argc == 2) {
		std::string fileName = argv[1];
		config.projectFileName = fileName;
	} else if (argc == 1) {
		config.projectFileName = "/home/android/dev/NovelTea/test.ntp";
	} else {
		for (int i = 0; i < argc; ++i) {
			std::string c = argv[i];
			std::cout << "arg" << i << ": " << c << std::endl;
		}
		return 1;
	}

	auto context = new NovelTea::Context(config);
	auto engine = new NovelTea::Engine(context);
	engine->setFramerateLocked(false);
	return engine->run();
}
