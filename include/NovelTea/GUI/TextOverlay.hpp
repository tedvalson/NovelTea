#ifndef NOVELTEA_TEXTOVERLAY_HPP
#define NOVELTEA_TEXTOVERLAY_HPP

#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/Utils.hpp>
#include <NovelTea/ActiveText.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

class TextOverlay : public sf::Drawable, public Hideable
{
public:
	TextOverlay();

	bool processEvent(const sf::Event& event);

	void show(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setText(const std::string &text);
	void setTextArray(const std::vector<std::string> &textArray);

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

protected:
	void ensureUpdate() const;
	bool gotoNextString(HideableCallback callback = nullptr);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
	mutable sf::FloatRect m_bounds;
	mutable bool m_needsUpdate;

	float m_alpha;

	TweenRectangleShape m_bg;
	ActiveText m_text;
	ActiveText m_oldText;

	size_t m_nextStringIndex;
	std::vector<std::string> m_strings;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTOVERLAY_HPP
