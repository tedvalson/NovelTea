#ifndef NOVELTEA_SFML_UTILS_HPP
#define NOVELTEA_SFML_UTILS_HPP

#include <SFML/Graphics/Color.hpp>
#include <NovelTea/TextTypes.hpp>

namespace sf {
class Text;
}

namespace NovelTea
{

sf::Color toColorSFML(const Color &color);
Color fromColorSFML(const sf::Color &color);

bool wrapText(sf::Text &text, float width);

} // namespace NovelTea

#endif // NOVELTEA_SFML_UTILS_HPP
