#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
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

#define REGISTER_ACCESSOR(className) \
	dukglue_register_function(m_context, SaveData::set<className>, "save"#className); \
	dukglue_register_function(m_context, SaveData::get<className>, "load"#className); \
	dukglue_register_function(m_context, SaveData::exists<className>, "exists"#className); \
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
}

void ScriptManager::runScriptId(const std::string &scriptId)
{
	auto script = Save.get<Script>(scriptId);
	if (!script->getId().empty())
	{
		// TODO: check if it's flagged global
		run(script->getContent());
	}
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

	// Action
	REGISTER_CONSTRUCTOR(Action);
	REGISTER_ACCESSOR(Action);

	// Cutscene
	REGISTER_CONSTRUCTOR(Cutscene);
	REGISTER_ACCESSOR(Cutscene);
	dukglue_register_method(m_context, &Cutscene::addSegment, "addSegment");
	dukglue_register_property(m_context, &Cutscene::getFullScreen, &Cutscene::setFullScreen, "fullScreen");
	dukglue_register_property(m_context, &Cutscene::getCanFastForward, &Cutscene::setCanFastForward, "canFastForward");
	dukglue_register_property(m_context, &Cutscene::getSpeedFactor, &Cutscene::setSpeedFactor, "speedFactor");

	// Dialogue
	REGISTER_CONSTRUCTOR(Dialogue);
	REGISTER_ACCESSOR(Dialogue);

	// Object
	REGISTER_CONSTRUCTOR(Object);
	REGISTER_ACCESSOR(Object);
	dukglue_register_property(m_context, &Object::getName, &Object::setName, "name");

	// Room
	REGISTER_CONSTRUCTOR(Room);
	REGISTER_ACCESSOR(Room);
	dukglue_register_property(m_context, &Room::getName, &Room::setName, "name");

	// Script
	REGISTER_CONSTRUCTOR(Script);
	REGISTER_ACCESSOR(Script);

	// Verb
	REGISTER_CONSTRUCTOR(Verb);
	REGISTER_ACCESSOR(Verb);
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
	dukglue_register_global(m_context, std::make_shared<Player>(), "Player");
	dukglue_register_property(m_context, &Player::getInventory, &Player::setInventory, "inv");
	dukglue_register_property(m_context, &Player::getRoomId, &Player::setRoomId, "roomId");
	dukglue_register_property(m_context, &Player::getRoom, &Player::setRoom, "room");

	// Script
	dukglue_register_global(m_context, this, "Script");
	dukglue_register_method(m_context, &ScriptManager::runScriptId, "run");
}

} // namespace NovelTea
