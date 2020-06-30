#ifndef NOVELTEA_VERBLIST_HPP
#define NOVELTEA_VERBLIST_HPP

#include <NovelTea/GUI/Scrollable.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <SFML/Graphics/View.hpp>
#include <functional>
#include <vector>

namespace NovelTea
{

using VerbSelectCallback = std::function<void(std::shared_ptr<Verb>)>;
using VerbShowHideCallback = std::function<void(bool)>;

class VerbList : public sf::Drawable, public Scrollable, public TweenTransformable<sf::Transformable>
{
public:
	static const int ALPHA = 11;

	VerbList();

	void update(float delta);
	bool processEvent(const sf::Event& event);

	void show();
	void hide();
	bool isVisible() const;

	void setVerbs(const std::vector<std::string> &verbs);
	void setVerbs(const std::shared_ptr<Object> &object);

	void setSelectCallback(VerbSelectCallback callback);
	void setShowHideCallback(VerbShowHideCallback callback);

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	void setValues(int tweenType, float *newValues) override;
	int getValues(int tweenType, float *returnValues) override;

	void repositionItems();

private:
	struct VerbOption {
		std::shared_ptr<Verb> verb;
		TweenText text;
	};

	float m_scrollPos;
	float m_margin;
	sf::Vector2f m_size;
	bool m_visible;
	bool m_isHiding;
	bool m_isShowing;

	ScrollBar m_scrollBar;
	TweenRectangleShape m_bg;

	sf::FloatRect m_bounds;

	mutable sf::Transform m_lastTransform;
	mutable sf::Vector2u m_targetSize;
	mutable sf::View m_view;
	std::vector<VerbOption> m_verbs;

	VerbSelectCallback m_selectCallback;
	VerbShowHideCallback m_showHideCallback;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_VERBLIST_HPP
