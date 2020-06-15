#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneSegment.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <fstream>
#include <iostream>

#define REGISTER_CONSTRUCTOR(class) \
	dukglue_register_function(m_context, std::make_shared<class>, "make"#class)

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

void ScriptManager::registerFunctions()
{
	dukglue_register_function(m_context, print, "print");
	dukglue_register_function(m_context, SaveData::saveVariables, "saveVariables");
	dukglue_register_function(m_context, SaveData::loadVariables, "loadVariables");
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

	// Cutscene
	REGISTER_CONSTRUCTOR(Cutscene);
	dukglue_register_method(m_context, &Cutscene::addSegment, "addSegment");
	dukglue_register_property(m_context, &Cutscene::getFullScreen, &Cutscene::setFullScreen, "fullScreen");
	dukglue_register_property(m_context, &Cutscene::getCanFastForward, &Cutscene::setCanFastForward, "canFastForward");
	dukglue_register_property(m_context, &Cutscene::getSpeedFactor, &Cutscene::setSpeedFactor, "speedFactor");
}

} // namespace NovelTea
