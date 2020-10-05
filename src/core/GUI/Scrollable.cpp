#include <NovelTea/GUI/Scrollable.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>

namespace NovelTea
{

Scrollable::Scrollable()
: m_scrollBar(nullptr)
{

}

Scrollable::~Scrollable()
{
	if (m_scrollBar)
		m_scrollBar->detachObject(this);
}

void Scrollable::attachScrollbar(ScrollBar *scrollbar)
{
	m_scrollBar = scrollbar;
}

void Scrollable::detachScrollbar()
{
	m_scrollBar = nullptr;
}

void Scrollable::updateScrollbar()
{
	if (m_scrollBar)
		m_scrollBar->markDirty();
}

} // namespace NovelTea
