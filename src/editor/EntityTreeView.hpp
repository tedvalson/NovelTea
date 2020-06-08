#ifndef ENTITYTREEVIEW_HPP
#define ENTITYTREEVIEW_HPP

#include <NovelTea/ProjectData.hpp>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QModelIndex>
#include <QVariant>


class EntityTreeView : public QTreeView
{
	Q_OBJECT

public:
	explicit EntityTreeView(QWidget *parent = 0);
	~EntityTreeView();

	void setModel(QAbstractItemModel *model);
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

private:
	QSortFilterProxyModel *proxyModel;
};

#endif // ENTITYTREEVIEW_HPP
