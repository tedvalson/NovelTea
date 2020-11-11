#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/ObjectList.hpp>
#include <NovelTea/PropertyList.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/GUI/Notification.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <fstream>
#include <iostream>

#define REGISTER_CONSTRUCTOR(className) \
	dukglue_register_function(m_context, std::make_shared<className>, "make"#className)

#define REGISTER_ENTITY(className) \
	dukglue_set_base_class<Entity, className>(m_context);  \
	dukglue_register_method(m_context, &className::prop, "prop"); \
	dukglue_register_method(m_context, &className::setProp, "setProp"); \
	dukglue_register_property(m_context, &className::getId, nullptr, "id"); \
	dukglue_register_method(m_context, &SaveData::get<className>, "load"#className); \
	dukglue_register_method(m_context, &SaveData::exists<className>, "exists"#className);

namespace
{
	void print(const std::string &str)
	{
		std::cout << str << std::endl;
	}
}

namespace NovelTea
{

ScriptManager::ScriptManager(Game *game)
	: m_context(nullptr)
	, m_game(game)
{
	reset();
}

ScriptManager::~ScriptManager()
{
	duk_destroy_heap(m_context);
}

void ScriptManager::reset()
{
	if (m_context)
		duk_destroy_heap(m_context);
	m_context = duk_create_heap_default();

	registerGlobals();
	registerFunctions();
	registerClasses();

	sf::FileInputStream file;
	std::string script;
#ifdef ANDROID
	if (file.open("core.js"))
#else
	if (file.open("/home/android/dev/NovelTea/core.js"))
#endif
	{
		script.resize(file.getSize());
		file.read(&script[0], script.size());
		run(script);
	}

	runAutorunScripts();
}

void ScriptManager::runScript(std::shared_ptr<Script> script)
{
	if (!script->getId().empty())
	{
		if (script->getGlobal())
			run(script->getContent());
		else
			runInClosure(script->getContent());
	}
}

void ScriptManager::runScriptId(const std::string &scriptId)
{
	runScript(m_game->getSaveData().get<Script>(scriptId));
}

bool ScriptManager::runActionScript(const std::string &verbId, const std::vector<std::string> &objectIds, const std::string &script)
{
	std::vector<std::shared_ptr<Object>> objects;
	for (auto &objectId : objectIds)
		objects.push_back(m_game->getSaveData().get<Object>(objectId));
	auto verb = m_game->getSaveData().get<Verb>(verbId);
	auto objectCount = objects.size();
	auto s = "function f(verb,object1,object2,object3,object4){\n"+script+"\nreturn true;}";
	try {
		if (objectCount == 0)
			return call<bool>(s, "f", verb);
		else if (objectCount == 1)
			return call<bool>(s, "f", verb, objects[0]);
		else if (objectCount == 2)
			return call<bool>(s, "f", verb, objects[0], objects[1]);
		else if (objectCount == 3)
			return call<bool>(s, "f", verb, objects[0], objects[1], objects[2]);
		else
			return call<bool>(s, "f", verb, objects[0], objects[1], objects[2], objects[3]);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
}

bool ScriptManager::runRoomScript(const std::string &roomId, const std::string &script)
{
	auto room = m_game->getSaveData().get<Room>(roomId);
	auto s = "function _f(room){\n"+script+"\nreturn true;}";
	try {
		return call<bool>(s, "_f", room);
	} catch (std::exception &e) {
		std::cerr << "runRoomScript (" << roomId << ") " << e.what() << std::endl;
		return false;
	}
}

void ScriptManager::registerFunctions()
{
	dukglue_register_function(m_context, print, "print");
	dukglue_register_function(m_context, Notification::spawn, "spawnNotification");
}

void ScriptManager::registerClasses()
{
	// ActiveText
	REGISTER_CONSTRUCTOR(ActiveText);
	dukglue_register_method(m_context, &ActiveText::addBlock, "addBlock");
	dukglue_register_method(m_context, &ActiveText::refresh, "refresh");

	// TextBlock
	REGISTER_CONSTRUCTOR(TextBlock);
	dukglue_register_method(m_context, &TextBlock::addFragment, "addFragment");

	// TextFragment
	REGISTER_CONSTRUCTOR(TextFragment);
	dukglue_register_property(m_context, &TextFragment::getText, &TextFragment::setText, "text");

	// ObjectList
	dukglue_register_method(m_context, &ObjectList::add, "add");
	dukglue_register_method(m_context, &ObjectList::remove, "remove");
	dukglue_register_method(m_context, &ObjectList::addId, "addId");
	dukglue_register_method(m_context, &ObjectList::removeId, "removeId");
	dukglue_register_method(m_context, &ObjectList::contains, "contains");
	dukglue_register_method(m_context, &ObjectList::containsId, "containsId");

	// PropertyList
	dukglue_register_method(m_context, &PropertyList::set, "set");
	dukglue_register_method(m_context, &PropertyList::get, "get");

	// Timer
	dukglue_register_method(m_context, &Timer::kill, "kill");
	dukglue_register_property(m_context, &Timer::getRepeat, &Timer::setRepeat, "repeat");
	dukglue_register_property(m_context, &Timer::getDuration, &Timer::setDuration, "duration");

	// Action
	REGISTER_CONSTRUCTOR(Action);
	REGISTER_ENTITY(Action);

	// Cutscene
	REGISTER_CONSTRUCTOR(Cutscene);
	REGISTER_ENTITY(Cutscene);
	dukglue_register_method(m_context, &Cutscene::addSegment, "addSegment");
	dukglue_register_property(m_context, &Cutscene::getFullScreen, &Cutscene::setFullScreen, "fullScreen");
	dukglue_register_property(m_context, &Cutscene::getCanFastForward, &Cutscene::setCanFastForward, "canFastForward");
	dukglue_register_property(m_context, &Cutscene::getSpeedFactor, &Cutscene::setSpeedFactor, "speedFactor");

	// Dialogue
	REGISTER_CONSTRUCTOR(Dialogue);
	REGISTER_ENTITY(Dialogue);

	// Object
	REGISTER_CONSTRUCTOR(Object);
	REGISTER_ENTITY(Object);
	dukglue_register_property(m_context, &Object::getName, &Object::setName, "name");

	// Room
	REGISTER_CONSTRUCTOR(Room);
	REGISTER_ENTITY(Room);
	dukglue_register_method(m_context, &Room::contains, "contains");
	dukglue_register_method(m_context, &Room::containsId, "containsId");
	dukglue_register_property(m_context, &Room::getObjectList, nullptr, "objects");

	// Script
	REGISTER_CONSTRUCTOR(Script);
	REGISTER_ENTITY(Script);

	// Verb
	REGISTER_CONSTRUCTOR(Verb);
	REGISTER_ENTITY(Verb);
}

void ScriptManager::registerGlobals()
{
	// Save
	dukglue_register_global(m_context, &m_game->getSaveData(), "Save");
	dukglue_register_method(m_context, &SaveData::set, "saveEntity");
	dukglue_register_method(m_context, &SaveData::save, "saveSlot");
	dukglue_register_method(m_context, &SaveData::load, "loadSlot");
	dukglue_register_method(m_context, &SaveData::resetRoomDescriptions, "resetRoomDescriptions");

	// Game
	dukglue_register_global(m_context, m_game, "Game");
	dukglue_register_method(m_context, &Game::pushNextEntity, "pushNext");
	dukglue_register_method(m_context, &Game::execMessageCallback, "message");
	dukglue_register_method(m_context, &Game::prop, "prop");
	dukglue_register_method(m_context, &Game::setProp, "setProp");
	dukglue_register_property(m_context, &Game::getObjectList, nullptr, "inventory");
	dukglue_register_property(m_context, &Game::getRoom, &Game::setRoom, "room");

	// Script
	dukglue_register_global(m_context, this, "Script");
	dukglue_register_method(m_context, &ScriptManager::runScriptId, "run");

	// TimerManager
	dukglue_register_global(m_context, &m_game->getTimerManager(), "Timer");
	dukglue_register_method(m_context, &TimerManager::start, "start");
	dukglue_register_method(m_context, &TimerManager::startRepeat, "startRepeat");
}

void ScriptManager::runAutorunScripts()
{
	if (m_game->getSaveData().isLoaded())
		for (auto &item : m_game->getSaveData().data()[Script::id].ObjectRange())
			checkAutorun(item.second);
	if (Proj.isLoaded())
		for (auto &item : ProjData[Script::id].ObjectRange())
			if (!m_game->getSaveData().data()[Script::id].hasKey(item.first))
				checkAutorun(item.second);
}

void ScriptManager::checkAutorun(const sj::JSON &j)
{
	auto script = m_game->getSaveData().get<Script>(j[ID::entityId].ToString());
	if (script->getAutorun())
		runScript(script);
}

} // namespace NovelTea
