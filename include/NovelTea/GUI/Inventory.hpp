#ifndef NOVELTEA_INVENTORY_HPP
#define NOVELTEA_INVENTORY_HPP

#include <NovelTea/GUI/Button.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

using InventoryCallback = std::function<void(const std::string&, float, float)>;

class Inventory : public sf::Drawable, public Scrollable, public Hideable
{
public:
	Inventory();

	bool update(float delta) override;
	bool processEvent(const sf::Event& event);

	void open();
	void close();
	bool isOpen();
	void refreshItems();
	void repositionItems();

	void setFontSizeMultiplier(float fontSizeMultiplier);
	void setScreenSize(const sf::Vector2f &size);

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setCallback(InventoryCallback callback);
	void setStartPosition(const sf::Vector2f &position);

	void setAlpha(float alpha) override;
	float getAlpha() const override;

protected:
	void ensureUpdate() const;
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_screenSize;
	mutable sf::FloatRect m_bounds;
	mutable bool m_needsUpdate;

	float m_margin;
	float m_itemHeight;
	float m_fontSizeMultiplier;

	float m_alpha;
	bool m_isOpen;

	mutable sf::Transform m_lastTransform;
	mutable sf::View m_view;

	mutable ScrollBar m_scrollBar;
	TweenRectangleShape m_bg;
	std::vector<std::unique_ptr<TweenText>> m_objectTexts;
	std::vector<std::string> m_objectIds;
	float m_scrollPos;
	sf::Vector2f m_startPosition;
	sf::Vector2f m_scrollAreaSize;

	InventoryCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_INVENTORY_HPP
