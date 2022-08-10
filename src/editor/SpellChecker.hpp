#ifndef SPELLCHECKER_HPP
#define SPELLCHECKER_HPP

#include <NovelTea/ContextObject.hpp>
#include <hunspell.hxx>
#include <memory>

class SpellChecker : public NovelTea::ContextObject
{
public:
	SpellChecker(NovelTea::Context *context);

	std::shared_ptr<Hunspell> hunspell();

private:
	void addEntityIds(const std::string &entityId);

	std::shared_ptr<Hunspell> m_hunspell;
};

#endif // SPELLCHECKER_HPP
