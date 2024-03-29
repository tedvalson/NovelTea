#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/ObjectList.hpp>
#include <NovelTea/PropertyList.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/RegexUtils.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/TextInput.hpp>
#include <NovelTea/TextLog.hpp>
#include <NovelTea/Timer.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/StringUtils.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>

#include <fstream>
#include <iostream>
#include <thread>

#define REGISTER_CONSTRUCTOR(className) \
	dukglue_register_method(m_context, &Game::makeContextObject<className>, "make"#className)

#define REGISTER_ENTITY(className) \
	dukglue_set_base_class<Entity, className>(m_context);  \
	dukglue_register_method(m_context, &className::prop, "prop"); \
	dukglue_register_method(m_context, &className::hasProp, "hasProp"); \
	dukglue_register_method(m_context, &className::setProp, "setProp"); \
	dukglue_register_method(m_context, &className::unsetProp, "unsetProp"); \
	dukglue_register_property(m_context, &className::getId, nullptr, "id"); \
	dukglue_register_method(m_context, &Game::get<className>, "load"#className); \
	dukglue_register_method(m_context, &Game::exists<className>, "exists"#className);

namespace
{
	void print(const std::string &str)
	{
		std::cout << str << std::endl;
	}

	void alert(const std::string &str)
	{
		NovelTea::err() << str << std::endl;
	}
}

namespace NovelTea
{

ScriptManager::ScriptManager(Context* context)
	: Subsystem(context)
	, m_context(nullptr)
	, m_randSeed(0)
{
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

	m_randEngine.seed(m_randSeed);

	registerGlobals();
	registerFunctions();
	registerClasses();

#ifdef ANDROID
	std::string script = getFileContents("core.js");
#else
	std::string script = getFileContents("/home/android/dev/NovelTea/res/assets/core.js");
#endif
	if (!script.empty())
		run(script);

	runAutorunScripts();
}

DukValue ScriptManager::runScript(std::shared_ptr<Script> script)
{
	try {
		setActiveEntity(script);
		return runInClosure<DukValue>(script->getContent());
	} catch (std::exception &e) {
		err() << "ScriptManager::runScript " << e.what() << std::endl;
		return dukglue_peval<DukValue>(m_context, "\"#Error# runScript\"");
	}
}

DukValue ScriptManager::runScriptId(const std::string &scriptId)
{
	return runScript(GGame->get<Script>(scriptId));
}

bool ScriptManager::runActionScript(const std::string &verbId, const std::vector<std::string> &objectIds, const std::string &script)
{
	std::vector<std::shared_ptr<Object>> objects;
	for (auto &objectId : objectIds)
		objects.push_back(GGame->get<Object>(objectId));
	auto verb = GGame->get<Verb>(verbId);
	auto objectCount = objects.size();
	auto s = "function f(verb,object1,object2,object3,object4){\n"+script+"\nreturn false;}";
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
		err() << e.what() << std::endl;
		return false;
	}
}

bool ScriptManager::runActionScript(const std::string &verbId, const std::string &verbIdOrig, const std::vector<std::string> &objectIds)
{
	auto verb = GGame->get<Verb>(verbId);
	auto script = verb->getScriptDefault();
	auto result = false;
	if (!script.empty())
		result = runActionScript(verbIdOrig, objectIds, script);

	if (!result) {
		// If script returns false and has no parent, execute "undefined action" script.
		if (verb->getParentId().empty())
			result = runActionScript(verbIdOrig, objectIds, ProjData[ID::scriptUndefinedAction].ToString());
		else
			result = runActionScript(verb->getParentId(), verbIdOrig, objectIds);
	}

	return result;
}

bool ScriptManager::runActionScript(const std::string &verbId, const std::vector<std::string> &objectIds)
{
	return runActionScript(verbId, verbId, objectIds);
}

void ScriptManager::setActiveEntity(std::shared_ptr<Entity> entity)
{
	call("", "_setActiveEntity", entity);
}

std::string ScriptManager::evalExpressions(const std::string &s)
{
	auto re = std::regex {R"(\{\{([\s\S]*?)\}\})"};

	return replaceRegex(s, re, [this, &s](const RegexIterator& it)
	{
		std::string result = "#ERROR#";
		std::string expr = (*it)[1];
		expr = strUseStrict + expr;
		try {
			auto r = dukglue_peval<DukValue>(m_context, expr.c_str());
			if (r.type() == DukValue::STRING)
				result = r.as_string();
			else if (r.type() == DukValue::UNDEFINED)
				result = "";
			else if (r.type() == DukValue::NUMBER) {
				float f = r.as_float();
				result = (f == round(f)) ? std::to_string(r.as_int()) : std::to_string(f);
			}
		} catch (std::exception &e) {
			std::cout << "Error replacing expression: " << s << std::endl << e.what() << std::endl;
		}
		return result;
	});
}

void ScriptManager::getTextInput(const std::string &message, const DukValue &func)
{
	GTextInput.run(message, [this, func](const std::string &text){
		try {
			dukglue_pcall<void>(m_context, func, text);
		} catch (std::exception e) {
			err() << "Error Script::getTextInput: " << e.what() << std::endl;
		}
	});
	// Block execution for platforms (eg. Android) that have non-blocking input methods
	while (!GTextInput.finished())
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void ScriptManager::randSeed(int seed)
{
	m_randSeed = seed;
	m_randEngine.seed(seed);
}

double ScriptManager::randGen()
{
	return m_uniformDist(m_randEngine);
}

void ScriptManager::registerFunctions()
{
	dukglue_register_function(m_context, alert, "alert");
	dukglue_register_function(m_context, print, "print");
}

void ScriptManager::registerClasses()
{
	// ObjectList
	dukglue_register_method(m_context, &ObjectList::add, "add");
	dukglue_register_method(m_context, &ObjectList::addId, "addId");
	dukglue_register_method(m_context, &ObjectList::addCount, "addCount");
	dukglue_register_method(m_context, &ObjectList::addIdCount, "addIdCount");
	dukglue_register_method(m_context, &ObjectList::remove, "remove");
	dukglue_register_method(m_context, &ObjectList::removeId, "removeId");
	dukglue_register_method(m_context, &ObjectList::removeCount, "removeCount");
	dukglue_register_method(m_context, &ObjectList::removeIdCount, "removeIdCount");
	dukglue_register_method(m_context, &ObjectList::contains, "contains");
	dukglue_register_method(m_context, &ObjectList::containsId, "containsId");
	dukglue_register_method(m_context, &ObjectList::containsCount, "containsCount");
	dukglue_register_method(m_context, &ObjectList::containsIdCount, "containsIdCount");
	dukglue_register_method(m_context, &ObjectList::count, "count");
	dukglue_register_method(m_context, &ObjectList::countId, "countId");
	dukglue_register_method(m_context, &ObjectList::getObjects, "getList");
	dukglue_register_method(m_context, &ObjectList::sync, "sync");

	// PropertyList
	dukglue_register_method(m_context, &PropertyList::set, "set");
	dukglue_register_method(m_context, &PropertyList::get, "get");
	dukglue_register_method(m_context, &PropertyList::sync, "sync");

	// Timer
	dukglue_register_method(m_context, &Timer::kill, "kill");
	dukglue_register_property(m_context, &Timer::getRepeat, &Timer::setRepeat, "repeat");
	dukglue_register_property(m_context, &Timer::getDuration, &Timer::setDuration, "duration");
	dukglue_register_property(m_context, &Timer::getTimePassed, &Timer::setTimePassed, "timePassed");

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
	dukglue_register_property(m_context, &Cutscene::getNextEntity, &Cutscene::setNextEntity, "nextEntity");

	// Dialogue
	REGISTER_CONSTRUCTOR(Dialogue);
	REGISTER_ENTITY(Dialogue);
	dukglue_register_method(m_context, &Dialogue::addSegment, "addSegment");
	dukglue_register_method(m_context, &Dialogue::getSegment, "getSegment");
	dukglue_register_method(m_context, &Dialogue::clearSegments, "clearSegments");
	dukglue_register_property(m_context, &Dialogue::getRootIndex, &Dialogue::setRootIndex, "rootIndex");
	dukglue_register_property(m_context, &Dialogue::getDefaultName, &Dialogue::setDefaultName, "defaultName");
	dukglue_register_property(m_context, &Dialogue::getNextEntity, &Dialogue::setNextEntity, "nextEntity");

	REGISTER_CONSTRUCTOR(DialogueSegment);
	dukglue_register_method(m_context, &DialogueSegment::appendChild, "appendChildId");
	dukglue_register_method(m_context, &DialogueSegment::clearChildren, "clearChildren");
	dukglue_register_property(m_context, &DialogueSegment::getId, nullptr, "id");
	dukglue_register_property(m_context, &DialogueSegment::getLinkId, &DialogueSegment::setLinkId, "linkId");
	dukglue_register_property(m_context, &DialogueSegment::getTypeInt, &DialogueSegment::setTypeInt, "type");
	dukglue_register_property(m_context, &DialogueSegment::getConditionalEnabled, &DialogueSegment::setConditionalEnabled, "conditionalEnabled");
	dukglue_register_property(m_context, &DialogueSegment::getScriptedText, &DialogueSegment::setScriptedText, "scriptedText");
	dukglue_register_property(m_context, &DialogueSegment::getScriptEnabled, &DialogueSegment::setScriptEnabled, "scriptEnabled");
	dukglue_register_property(m_context, &DialogueSegment::getShowOnce, &DialogueSegment::setShowOnce, "showOnce");
	dukglue_register_property(m_context, &DialogueSegment::getConditionScript, &DialogueSegment::setConditionScript, "conditionScript");
	dukglue_register_property(m_context, &DialogueSegment::getScript, &DialogueSegment::setScript, "script");
	dukglue_register_property(m_context, &DialogueSegment::getTextRaw, &DialogueSegment::setTextRaw, "text");
	dukglue_register_property(m_context, &DialogueSegment::getChildrenIds, &DialogueSegment::setChildrenIds, "childIds");

	// Map
	REGISTER_CONSTRUCTOR(Map);
	REGISTER_ENTITY(Map);
	dukglue_register_method(m_context, &Map::scriptAddRoom, "addRoom");
	dukglue_register_method(m_context, &Map::scriptAddConnection, "addConnection");
	dukglue_register_property(m_context, &Map::getDefaultRoomScript, &Map::setDefaultRoomScript, "defaultRoomScript");
	dukglue_register_property(m_context, &Map::getDefaultPathScript, &Map::setDefaultPathScript, "defaultPathScript");

	// Object
	REGISTER_CONSTRUCTOR(Object);
	REGISTER_ENTITY(Object);
	dukglue_register_property(m_context, &Object::getName, &Object::setName, "name");

	// Room
	REGISTER_CONSTRUCTOR(Room);
	REGISTER_ENTITY(Room);
	dukglue_register_method(m_context, &Room::contains, "contains");
	dukglue_register_method(m_context, &Room::containsId, "containsId");
	dukglue_register_method(m_context, &Room::sync, "sync");
	dukglue_register_method(m_context, &Room::setPathEntity, "setPathEntity");
	dukglue_register_method(m_context, &Room::setPathScript, "setPathScript");
	dukglue_register_method(m_context, &Room::enablePath, "enablePath");
	dukglue_register_method(m_context, &Room::disablePath, "disablePath");
	dukglue_register_property(m_context, &Room::getVisitCount, &Room::setVisitCount, "visitCount");
	dukglue_register_property(m_context, &Room::getObjectList, nullptr, "objects");
	dukglue_register_property(m_context, &Room::getScriptAfterEnter, &Room::setScriptAfterEnter, "scriptAfterEnter");
	dukglue_register_property(m_context, &Room::getScriptAfterLeave, &Room::setScriptAfterLeave, "scriptAfterLeave");
	dukglue_register_property(m_context, &Room::getScriptBeforeEnter, &Room::setScriptBeforeEnter, "scriptBeforeEnter");
	dukglue_register_property(m_context, &Room::getScriptBeforeLeave, &Room::setScriptBeforeLeave, "scriptBeforeLeave");
	dukglue_register_property(m_context, &Room::getDescription, &Room::setDescription, "description");
	dukglue_register_property(m_context, &Room::getDescriptionRaw, &Room::setDescriptionRaw, "rawDescription");

	// Script
	REGISTER_CONSTRUCTOR(Script);
	REGISTER_ENTITY(Script);
	dukglue_register_property(m_context, &Script::getAutorun, &Script::setAutorun, "autorun");
	dukglue_register_property(m_context, &Script::getContent, &Script::setContent, "content");

	// Verb
	REGISTER_CONSTRUCTOR(Verb);
	REGISTER_ENTITY(Verb);
}

void ScriptManager::registerGlobals()
{
	// Save
	dukglue_register_global(m_context, GSave, "Save");
	dukglue_register_method(m_context, &SaveData::resetRoomDescriptions, "resetRoomDescriptions");

	// Game
	dukglue_register_global(m_context, GGame, "Game");
	dukglue_register_method(m_context, &Game::pushNextEntity, "pushNext");
	dukglue_register_method(m_context, &Game::execMessageCallback, "message");
	dukglue_register_method(m_context, &Game::execMessageCallbackLog, "messageLog");
	dukglue_register_method(m_context, &Game::spawnNotification, "spawnNotification");
	dukglue_register_method(m_context, &Game::prop, "prop");
	dukglue_register_method(m_context, &Game::setProp, "setProp");
	dukglue_register_method(m_context, &Game::save, "save");
	dukglue_register_method(m_context, &Game::load, "load");
	dukglue_register_method(m_context, &Game::autosave, "autosave");
	dukglue_register_method(m_context, &Game::quit, "quit");
	dukglue_register_method(m_context, &Game::set, "saveEntity");
	dukglue_register_property(m_context, &Game::getObjectList, nullptr, "inventory");
	dukglue_register_property(m_context, &Game::getRoom, nullptr, "room");
	dukglue_register_property(m_context, &Game::getMapId, &Game::setMapId, "mapId");
	dukglue_register_property(m_context, &Game::getMinimapEnabled, &Game::setMinimapEnabled, "minimap");
	dukglue_register_property(m_context, &Game::getNavigationEnabled, &Game::setNavigationEnabled, "navigation");
	dukglue_register_property(m_context, &Game::getSaveEnabled, &Game::setSaveEnabled, "saveEnabled");

	// Script
	dukglue_register_global(m_context, this, "Script");
	dukglue_register_method(m_context, &ScriptManager::evalExpressions, "evalExpressions");
	dukglue_register_method(m_context, &ScriptManager::getTextInput, "getTextInput");
	dukglue_register_method(m_context, &ScriptManager::runScriptId, "run");
	dukglue_register_method(m_context, &ScriptManager::randGen, "rand");
	dukglue_register_method(m_context, &ScriptManager::randSeed, "seed");

	// TextLog
	dukglue_register_global(m_context, GTextLog, "Log");
	dukglue_register_method(m_context, &TextLog::pushScript, "push");

	// TimerManager
	dukglue_register_global(m_context, TimerMan, "Timer");
	dukglue_register_method(m_context, &TimerManager::start, "start");
	dukglue_register_method(m_context, &TimerManager::startRepeat, "startRepeat");
}

void ScriptManager::runAutorunScripts()
{
	if (GSave->isLoaded())
		for (auto &item : GSaveData[Script::id].ObjectRange())
			checkAutorun(item.second);
	if (Proj->isLoaded())
		for (auto &item : ProjData[Script::id].ObjectRange())
			if (!GSaveData[Script::id].hasKey(item.first))
				checkAutorun(item.second);
}

void ScriptManager::checkAutorun(const sj::JSON &j)
{
	auto entityId = j[ID::entityId].ToString();
	auto script = GGame->get<Script>(entityId);
	if (script->getAutorun())
		runScript(script);
}

} // namespace NovelTea
