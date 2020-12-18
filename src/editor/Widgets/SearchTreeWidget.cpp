#include "SearchTreeWidget.hpp"
#include <QEvent>

SearchTreeWidget::SearchTreeWidget(QWidget *parent)
: QTreeWidget(parent)
{
	//
}

bool SearchTreeWidget::event(QEvent *event)
{
	return QTreeWidget::event(event);
}
