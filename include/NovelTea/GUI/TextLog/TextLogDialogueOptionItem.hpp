#ifndef NOVELTEA_TEXTLOGDIALOGUEOPTIONITEM_HPP
#define NOVELTEA_TEXTLOGDIALOGUEOPTIONITEM_HPP

#include <NovelTea/GUI/TextLog/TextLogItem.hpp>
#include <NovelTea/ActiveText.hpp>

namespace NovelTea
{

class TextLogDialogueOptionItem : public TextLogItem
{
public:
	TextLogDialogueOptionItem(Context *context, const std::string &text);
	virtual void setAlpha(float alpha) override;
	virtual void setWidth(float width) override;

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	ActiveText m_text;
	TweenNinePatch m_bg;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOGDIALOGUEOPTIONITEM_HPP
