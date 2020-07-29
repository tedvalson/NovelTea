#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>

namespace NovelTea
{

TextFragment::TextFragment()
{
}

json TextFragment::toJson() const
{
	return sj::Array(
		Proj.addTextFormat(m_textFormat),
		m_text
	);
}

bool TextFragment::fromJson(const json &j)
{
	m_textFormat = Proj.textFormat(j[0].ToInt());
	m_text = j[1].ToString();
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

void TextFragment::setText(const std::string &text)
{
	m_text = text;
}

const std::string &TextFragment::getText() const
{
	return m_text;
}

} // namespace NovelTea
