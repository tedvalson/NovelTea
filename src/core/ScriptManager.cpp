#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/ObjectList.hpp>
#include <NovelTea/Player.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <fstream>
#include <iostream>

#define REGISTER_CONSTRUCTOR(className) \
	dukglue_register_function(m_context, std::make_shared<className>, "make"#className)

#define REGISTER_ENTITY(className) \
	dukglue_set_base_class<Entity, className>(m_context);  \
	dukglue_register_function(m_context, SaveData::set<className>, "save"#className); \
	dukglue_register_function(m_context, SaveData::get<className>, "load"#className); \
	dukglue_register_function(m_context, SaveData::exists<className>, "exists"#className); \
	dukglue_register_method(m_context, &className::prop, "prop"); \
	dukglue_register_method(m_context, &className::setProp, "setProp"); \
	dukglue_register_property(m_context, &className::getId, nullptr, "id");

namespace
{
	void print(const std::string &str)
	{
		std::cout << str << std::endl;
	}
}

namespace NovelTea
{

ScriptManager::ScriptManager()
	: m_context(nullptr)
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
	if (file.open("/home/android/dev/NovelTea/core.js"))
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
	runScript(Save.get<Script>(scriptId));
}

void ScriptManager::registerFunctions()
{
	dukglue_register_function(m_context, print, "print");
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
	dukglue_register_global(m_context, &SaveData::instance(), "Save");
	dukglue_register_method(m_context, &SaveData::save, "saveSlot");
	dukglue_register_method(m_context, &SaveData::load, "loadSlot");
	dukglue_register_method(m_context, &SaveData::writeVariables, "writeVariables");
	dukglue_register_method(m_context, &SaveData::readVariables, "readVariables");

	// Player
	dukglue_register_global(m_context, &Player::instance(), "Player");
	dukglue_register_property(m_context, &Player::getObjectList, nullptr, "inventory");
	dukglue_register_property(m_context, &Player::getRoom, &Player::setRoom, "room");

	// Script
	dukglue_register_global(m_context, this, "Script");
	dukglue_register_method(m_context, &ScriptManager::runScriptId, "run");
}

void ScriptManager::runAutorunScripts()
{
	if (Save.isLoaded())
		for (auto &jitem : Save.data()[Script::id])
			checkAutorun(jitem);
	if (Proj.isLoaded())
		for (auto &item : ProjData[Script::id].items())
			if (!Save.data()[Script::id].contains(item.key()))
				checkAutorun(item.value());
}

void ScriptManager::checkAutorun(const nlohmann::json &j)
{
	auto script = Save.get<Script>(j[0]);
	if (script->getAutorun())
		runScript(script);
}

} // namespace NovelTea
