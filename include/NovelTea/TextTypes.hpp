#ifndef NOVELTEA_TEXTTYPES_HPP
#define NOVELTEA_TEXTTYPES_HPP

#include <TweenEngine/TweenEquations.h>
#include <SFML/Graphics/Text.hpp>

namespace NovelTea
{

enum class TextStyleType : int {
	None = 0,
	Animation = 1,
	Bold,
	BorderColor,
	BorderSize,
	Color,
	Diff,
	Font,
	Italic,
	Object,
	PBreak,
	Size,
	Shader,
	XOffset,
	YOffset,
};

enum class TextEffect : int {
	None = 0,
	Fade = 1,
	FadeAcross,
	Pop,
};

struct AnimationProperties {
	TextEffect type = TextEffect::None;
	TweenEngine::TweenEquation* equation = &TweenEngine::TweenEquations::easeInOutQuad;
	std::string value;
	int duration  = 0;
	int delay     = 0;
	int loopCount = 0;
	int loopDelay = 0;
	float speed   = 1.f;
	bool loopYoyo     = true;
	bool skippable    = true;
	bool waitForClick = false;
};

struct TextProperties {
	std::string fontAlias;
	std::string objectId;
	std::string vertexShaderId;
	std::string fragShaderId;
	std::map<std::string, float> shaderUniforms;
	int xOffset = 0;
	int yOffset = 0;
	sf::Uint32 fontSize = 12;
	sf::Uint32 fontStyle = sf::Text::Regular;
	sf::Color color = sf::Color::Black;
	sf::Color outlineColor = sf::Color::Black;
	float outlineThickness = 0.f;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTTYPES_HPP
