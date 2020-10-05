#ifndef NOVELTEA_SCROLLABLE_HPP
#define NOVELTEA_SCROLLABLE_HPP

#include <SFML/System/Vector2.hpp>

namespace NovelTea
{

class ScrollBar;

class Scrollable {
friend class ScrollBar;
public:
	Scrollable();
	~Scrollable();
	virtual void setScroll(float position) = 0;
	virtual float getScroll() = 0;
	virtual const sf::Vector2f &getScrollSize() = 0;

protected:
	void attachScrollbar(ScrollBar *scrollbar);
	void detachScrollbar();
	void updateScrollbar();

private:
	ScrollBar *m_scrollBar;
};

} // namespace NovelTea

#endif // NOVELTEA_SCROLLABLE_HPP
