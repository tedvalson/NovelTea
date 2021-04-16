#ifndef ENTITYTREEVIEW_HPP
#define ENTITYTREEVIEW_HPP

#include "EntitySortFilterProxyModel.hpp"
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

	void setFilterRegExp(const QString &pattern);

private:
	EntitySortFilterProxyModel *proxyModel;
};

#endif // ENTITYTREEVIEW_HPP
