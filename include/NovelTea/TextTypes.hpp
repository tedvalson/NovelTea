#ifndef NOVELTEA_TEXTTYPES_HPP
#define NOVELTEA_TEXTTYPES_HPP

#include <SFML/Graphics/Text.hpp>

namespace NovelTea
{

enum class TextStyleType : int {
	None = 0,
	Bold = 1,
	Italic,
	Color,
	Font,
	Animation,
	Object,
	Size,
};

enum class TextEffect : int {
	None = 0,
	Fade = 1,
	FadeAcross,
};

struct AnimationProperties {
	TextEffect type = TextEffect::None;
	int duration = 1000;
	int delay    = 1000;
	float speed  = 1.f;
	bool skippable = true;
	bool waitForClick = false;
};

struct TextProperties {
	std::string fontAlias;
	std::string objectId;
	sf::Uint32 fontSize = 12;
	sf::Uint32 fontStyle = sf::Text::Regular;
	sf::Color color = sf::Color::Black;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTTYPES_HPP
