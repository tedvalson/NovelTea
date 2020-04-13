#include <NovelTea/TextFormat.hpp>
#include <iostream>

namespace NovelTea
{

TextFormat::TextFormat()
{
}

bool TextFormat::operator==(const TextFormat &format) const
{
	return (_bold == format._bold) &&
			(_italic == format._italic) &&
			(_underline == format._underline) &&
			(_size == format._size) &&
			(_color == format._color);
}

json TextFormat::toJson() const
{
	auto j = json::array({
		bold(),
		italic(),
		underline(),
		size(),
		color()
	});
	return j;
}

bool TextFormat::fromJson(const json &j)
{
	bold(j[0]);
	italic(j[1]);
	underline(j[2]);
	size(j[3]);
	color(j[4]);
}

void TextFormat::bold(bool val)
{
	_bold = val;
}

bool TextFormat::bold() const
{
	return _bold;
}

void TextFormat::italic(bool val)
{
	_italic = val;
}

bool TextFormat::italic() const
{
	return _italic;
}

void TextFormat::underline(bool val)
{
	_underline = val;
}

bool TextFormat::underline() const
{
	return _underline;
}

void TextFormat::size(int val)
{
	_size = val;
}

int TextFormat::size() const
{
	return _size;
}

void TextFormat::color(const sf::Color val)
{
	_color = val;
}

const sf::Color &TextFormat::color() const
{
	return _color;
}

} // namespace NovelTea
