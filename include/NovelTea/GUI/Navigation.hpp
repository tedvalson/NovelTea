#ifndef NOVELTEA_NAVIGATION_HPP
#define NOVELTEA_NAVIGATION_HPP

#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

using NavigationCallback = std::function<void(const json &jentity)>;

class Navigation : public sf::Drawable, public Hideable
{
public:
	Navigation();
	~Navigation();

	bool processEvent(const sf::Event& event);

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setPaths(const json &value);
	const json &getPaths() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setCallback(NavigationCallback callback);

	void setAlpha(float alpha) override;
	float getAlpha() const override;

protected:
	void ensureUpdate() const;
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
	mutable sf::FloatRect m_bounds;
	mutable bool m_needsUpdate;

	float m_alpha;
	json m_paths;

	mutable std::vector<std::unique_ptr<TweenRectangleShape>> m_buttons;

	NavigationCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_NAVIGATION_HPP
