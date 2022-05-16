#ifndef NOVELTEA_TEXTLOGDIALOGUETEXTITEM_HPP
#define NOVELTEA_TEXTLOGDIALOGUETEXTITEM_HPP

#include <NovelTea/GUI/TextLog/TextLogItem.hpp>
#include <NovelTea/ActiveText.hpp>

namespace NovelTea
{

class TextLogDialogueTextItem : public TextLogItem
{
public:
	TextLogDialogueTextItem(const std::string &name, const std::string &text);
	virtual void setAlpha(float alpha) override;
	virtual void setWidth(float width) override;

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	bool m_hasName;
	ActiveText m_text;
	ActiveText m_textName;
	TweenNinePatch m_bg;
	TweenNinePatch m_bgName;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOGDIALOGUETEXTITEM_HPP
