#ifndef NOVELTEA_VERBLIST_HPP
#define NOVELTEA_VERBLIST_HPP

#include <NovelTea/GUI/Scrollable.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <SFML/Graphics/View.hpp>
#include <functional>
#include <vector>

namespace NovelTea
{

using VerbSelectCallback = std::function<void(const std::string&)>;
using VerbShowHideCallback = std::function<void(bool)>;

class VerbList : public sf::Drawable, public Scrollable, public Hideable
{
public:
	VerbList();

	bool update(float delta) override;
	bool processEvent(const sf::Event& event);

	void refreshItems();

	void setScreenSize(const sf::Vector2f &size);
	const sf::Vector2f &getScreenSize() const;

	void show(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setVerbs(const std::vector<std::string> &verbIds);
	void setVerbs(const std::string &objectId);

	void setSelectCallback(VerbSelectCallback callback);
	void setShowHideCallback(VerbShowHideCallback callback);

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	void setPositionBounded(const sf::Vector2f& position);
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	void repositionItems();
	void addVerbOption(const std::string &verbId);

private:
	struct VerbOption {
		std::string verbId;
		TweenText text;
	};

	float m_scrollPos;
	float m_margin;
	float m_itemHeight;
	sf::Vector2f m_screenSize;
	sf::Vector2f m_scrollAreaSize;

	ScrollBar m_scrollBar;
	TweenRectangleShape m_bg;
	TweenText m_text;

	sf::FloatRect m_bounds;

	mutable sf::Transform m_lastTransform;
	mutable sf::View m_view;
	std::vector<VerbOption> m_verbs;

	VerbSelectCallback m_selectCallback;
	VerbShowHideCallback m_showHideCallback;
};

} // namespace NovelTea

#endif // NOVELTEA_VERBLIST_HPP
