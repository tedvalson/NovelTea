#ifndef NOVELTEA_TEXTLOGGENERICITEM_HPP
#define NOVELTEA_TEXTLOGGENERICITEM_HPP

#include <NovelTea/GUI/TextLog/TextLogItem.hpp>
#include <NovelTea/ActiveText.hpp>

namespace NovelTea
{

class TextLogGenericItem : public TextLogItem
{
public:
	TextLogGenericItem(Context *context, const std::string &text);
	virtual void setAlpha(float alpha) override;
	virtual void setWidth(float width) override;

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	ActiveText m_text;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOGGENERICITEM_HPP
