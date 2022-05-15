#ifndef NOVELTEA_TEXTLOGRENDERER_HPP
#define NOVELTEA_TEXTLOGRENDERER_HPP

#include <NovelTea/GUI/TextLog/TextLogItem.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/ActiveText.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

class TextLogRenderer : public sf::Drawable, public Hideable, public Scrollable
{
public:
	TextLogRenderer();

	bool processEvent(const sf::Event& event);
	bool update(float delta) override;

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;
	void repositionText();

	void setFontSizeMultiplier(float fontSizeMultiplier);

	void show(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

protected:
	void ensureUpdate() const;
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
	mutable bool m_needsUpdate;
	bool m_mousePressed;

	mutable sf::View m_view;
	float m_padding;
	float m_alpha;
	float m_fontSizeMultiplier;

	float m_scrollPos;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_scrollBar;

	TweenRectangleShape m_bg;
	std::vector<std::unique_ptr<TextLogItem>> m_items;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTLOGRENDERER_HPP
