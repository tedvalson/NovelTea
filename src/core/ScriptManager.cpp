#include <NovelTea/ScriptManager.hpp>

#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>

#include <fstream>
#include <iostream>

namespace NovelTea
{

ScriptManager::ScriptManager()
{
	m_context = duk_create_heap_default();
	registerClasses();
}

ScriptManager::~ScriptManager()
{
	duk_destroy_heap(m_context);
}

void ScriptManager::registerClasses()
{
	// ActiveText
	dukglue_register_constructor<ActiveText>(m_context, "ActiveText");
	dukglue_register_method(m_context, &ActiveText::addBlock, "addBlock");
	// TextBlock
	dukglue_register_constructor<TextBlock>(m_context, "TextBlock");
	dukglue_register_method(m_context, &TextBlock::addFragment, "addFragment");
	// TextFragment
	dukglue_register_constructor<TextFragment>(m_context, "TextFragment");
	dukglue_register_method(m_context, &TextFragment::setText, "setText");
}

void ScriptManager::run(const std::string &script)
{
	run<void>(script);
}

ScriptManager &ScriptManager::instance()
{
	static ScriptManager obj;
	return obj;
}

} // namespace NovelTea
