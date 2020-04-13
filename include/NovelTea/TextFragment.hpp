#ifndef NOVELTEA_TEXTFRAGMENT_HPP
#define NOVELTEA_TEXTFRAGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/TextFormat.hpp>
#include <SFML/System/String.hpp>

namespace NovelTea
{

class TextFragment : public JsonSerializable
{
public:
	TextFragment();
	virtual json toJson() const;
	virtual bool fromJson(const json &j);

	void setTextFormat(const TextFormat &textFormat);
	const TextFormat &getTextFormat() const;

	void setText(const sf::String &text);
	const sf::String &getText() const;

private:
	TextFormat m_textFormat;
	sf::String m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTFRAGMENT_HPP
