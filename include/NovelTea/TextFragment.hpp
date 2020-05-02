#ifndef NOVELTEA_TEXTFRAGMENT_HPP
#define NOVELTEA_TEXTFRAGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/TextFormat.hpp>

namespace NovelTea
{

class TextFragment : public JsonSerializable
{
public:
	TextFragment();
	json toJson() const override;
	bool fromJson(const json &j) override;

	void setTextFormat(const TextFormat &textFormat);
	const TextFormat &getTextFormat() const;

	void setText(const std::string &text);
	const std::string &getText() const;

private:
	TextFormat m_textFormat;
	std::string m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTFRAGMENT_HPP
