#ifndef SEARCHTREEWIDGET_HPP
#define SEARCHTREEWIDGET_HPP

#include <QTreeWidget>

class SearchTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	SearchTreeWidget(QWidget *parent = 0);

public slots:

protected:
	bool event(QEvent *event) override;

private:
};

#endif // SEARCHTREEWIDGET_HPP
