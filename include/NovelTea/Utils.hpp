#ifndef NOVELTEA_UTILS_HPP
#define NOVELTEA_UTILS_HPP

#include <SFML/Graphics/Text.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <vector>
#include <memory>
#include <string>

#define ADD_ACCESSOR(type, name, var) \
	virtual void set##name(const type & arg){var = arg;} \
	virtual const type & get##name() const {return var;}

#define SET_ALPHA(getFunc, setFunc, maxValue) \
	color = getFunc(); \
	color.a = std::max(std::min(newValues[0] * maxValue / 255.f, 255.f), 0.f); \
	setFunc(color);

template<typename T>
using SharedVector = std::vector<std::shared_ptr<T>>;
template<typename T>
using UniqueVector = std::vector<std::unique_ptr<T>>;

namespace NovelTea
{

std::vector<std::string> split(const std::string &text, const std::string &delimiter = "\n");
std::string join(const std::vector<std::string> &words, const std::string &delimiter = "\n");
std::string replace(const std::string &text, const std::string &textToReplace, const std::string &replacement = "");
bool wrapText(sf::Text &text, float width);

EntityType entityIdToType(const std::string &entityId);
std::string entityTypeToId(EntityType entityType);

} // namespace NovelTea

#endif // NOVELTEA_UTILS_HPP
