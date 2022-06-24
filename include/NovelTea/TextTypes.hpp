#ifndef NOVELTEA_TEXTTYPES_HPP
#define NOVELTEA_TEXTTYPES_HPP

#include <SFML/Graphics/Text.hpp>

namespace NovelTea
{

enum class TextStyleType : int {
	None = 0,
	Animation = 1,
	Bold,
	Color,
	Diff,
	Font,
	Italic,
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
	int duration = 0;
	int delay    = 0;
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
	sf::Color outlineColor = sf::Color::Black;
	float outlineThickness = 0.f;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTTYPES_HPP
