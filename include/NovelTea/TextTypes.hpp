#ifndef NOVELTEA_TEXTTYPES_HPP
#define NOVELTEA_TEXTTYPES_HPP

#include <TweenEngine/TweenEquations.h>
#include <cstdint>
#include <string>
#include <map>

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
	Glow,
	Nod,
	Shake,
	Test,
	Tremble,
	Pop,
};

enum FontStyle {
	FontRegular       = 0,
	FontBold          = 1 << 0,
	FontItalic        = 1 << 1,
	FontUnderlined    = 1 << 2,
	FontStrikeThrough = 1 << 3
};

struct Color {
	Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
		: r(r), g(g), b(b), a(a) {}
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	std::uint8_t a;
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
	unsigned int fontSize = 12;
	unsigned int fontStyle = FontRegular;
	Color color = {0, 0, 0};
	Color outlineColor = {0, 0, 0};
	float outlineThickness = 0.f;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTTYPES_HPP
