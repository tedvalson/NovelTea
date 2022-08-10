#include "SpellChecker.hpp"
#include <NovelTea/Game.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/StringUtils.hpp>
#include <QCoreApplication>
#include <iostream>


SpellChecker::SpellChecker(NovelTea::Context *context)
	: NovelTea::ContextObject(context)
{
	auto dictPath = QCoreApplication::applicationDirPath().toStdString() + "/dicts/";
	auto affPath = dictPath + "en_US.aff";
	auto dPath = dictPath + "en_US.dic";
	m_hunspell = std::make_shared<Hunspell>(affPath.c_str(), dPath.c_str());

	addEntityIds(NovelTea::ID::Action);
	addEntityIds(NovelTea::ID::Dialogue);
	addEntityIds(NovelTea::ID::Cutscene);
	addEntityIds(NovelTea::ID::Object);
	addEntityIds(NovelTea::ID::Room);
	addEntityIds(NovelTea::ID::Script);
	addEntityIds(NovelTea::ID::Verb);

	for (auto &jitem : ProjData[NovelTea::ID::spellWhitelist].ArrayRange())
		m_hunspell->add(jitem.ToString());
}

std::shared_ptr<Hunspell> SpellChecker::hunspell()
{
	return m_hunspell;
}

// Add entity IDs to white list.
void SpellChecker::addEntityIds(const std::string &entityId)
{
	for (auto &item : ProjData[entityId].ObjectRange())
		for (auto &word : NovelTea::split(item.first, " "))
			m_hunspell->add(word);
}
