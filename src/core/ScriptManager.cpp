#include <NovelTea/ScriptManager.hpp>

#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>

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
{
	m_context = duk_create_heap_default();

	registerFunctions();
	registerClasses();
}

ScriptManager::~ScriptManager()
{
	duk_destroy_heap(m_context);
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
}

ScriptManager &ScriptManager::instance()
{
	static ScriptManager obj;
	return obj;
}

} // namespace NovelTea
