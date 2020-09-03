#ifndef NOVELTEA_UTILS_HPP
#define NOVELTEA_UTILS_HPP

#include <SFML/Graphics/Text.hpp>
#include <vector>
#include <string>

#define ADD_ACCESSOR(type, name, var) \
	virtual void set##name(const type & arg){var = arg;} \
	virtual const type & get##name() const {return var;}

#define SET_ALPHA(getFunc, setFunc, maxValue) \
	color = getFunc(); \
	color.a = std::max(std::min(newValues[0] * maxValue / 255.f, 255.f), 0.f); \
	setFunc(color);

namespace NovelTea
{

std::vector<std::string> split(const std::string &text, const std::string &delimiter = "\n");
bool wrapText(sf::Text &text, float width);

} // namespace NovelTea

#endif // NOVELTEA_UTILS_HPP
