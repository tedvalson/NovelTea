#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>

namespace NovelTea
{

TextFragment::TextFragment()
{
}

json TextFragment::toJson() const
{
	return json::array({
		Proj.addTextFormat(m_textFormat),
		m_text.toAnsiString()
	});
}

bool TextFragment::fromJson(const json &j)
{
	m_textFormat = Proj.textFormat(j[0]);
	m_text = j[1].get<std::string>();
	return true;
}

void TextFragment::setTextFormat(const TextFormat &textFormat)
{
	m_textFormat = textFormat;
}

const TextFormat &TextFragment::getTextFormat() const
{
	return m_textFormat;
}

void TextFragment::setText(const sf::String &text)
{
	m_text = text;
}

const sf::String &TextFragment::getText() const
{
	return m_text;
}

} // namespace NovelTea
