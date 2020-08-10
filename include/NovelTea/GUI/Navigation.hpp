#ifndef NOVELTEA_NAVIGATION_HPP
#define NOVELTEA_NAVIGATION_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

using NavigationCallback = std::function<void(const json &jentity)>;

class Navigation : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	static const int ALPHA = 11;

	Navigation();
	~Navigation();

	void update(float delta);
	bool processEvent(const sf::Event& event);

	void show();
	void hide();
	bool isVisible() const;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setPaths(const json &value);
	const json &getPaths() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setCallback(NavigationCallback callback);

protected:
	void ensureUpdate() const;
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	void setValues(int tweenType, float *newValues) override;
	int getValues(int tweenType, float *returnValues) override;

private:
	sf::Vector2f m_size;
	mutable sf::FloatRect m_bounds;
	mutable bool m_needsUpdate;

	TweenEngine::TweenManager m_tweenManager;
	float m_alpha;

	bool m_visible;
	bool m_isHiding;
	bool m_isShowing;

	json m_paths;

	mutable std::vector<std::unique_ptr<TweenRectangleShape>> m_buttons;

	NavigationCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_NAVIGATION_HPP
