#include <NovelTea/SFML/Utils.hpp>
#include <NovelTea/StringUtils.hpp>
#include <SFML/Graphics/Text.hpp>

namespace NovelTea
{

sf::Color toColorSFML(const Color &c)
{
	return std::move(sf::Color(c.r, c.g, c.b, c.a));
}

Color fromColorSFML(const sf::Color &c)
{
	return std::move(Color(c.r, c.g, c.b, c.a));
}

bool wrapText(sf::Text &text, float width)
{
	if (text.getLocalBounds().width <= width)
		return false;

	auto s = text.getString().toAnsiString();
	auto words = split(s, " ");
	auto processedWidth = 0.f;
	int pos = 0;
	std::string out;
	sf::Vector2f lastWordPos;
	for (auto &word : words)
	{
		auto p = text.findCharacterPos(pos + word.size());
		if (p.x - processedWidth > width)
		{
			out += "\n" + word + " ";
			pos += word.size() + 1;
			processedWidth += lastWordPos.x - processedWidth;
		} else {
			out += word + " ";
			pos += word.size() + 1;
		}
		lastWordPos = p;
	}

	text.setString(out);
	return true;
}

} // namespace NovelTea
