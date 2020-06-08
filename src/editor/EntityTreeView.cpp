#include "EntityTreeView.hpp"
#include <iostream>

EntityTreeView::EntityTreeView(QWidget *parent)
	: QTreeView(parent)
	, proxyModel(new QSortFilterProxyModel)
{

}

EntityTreeView::~EntityTreeView()
{
	delete proxyModel;
}

void EntityTreeView::setModel(QAbstractItemModel *model)
{
	proxyModel->setSourceModel(model);
	proxyModel->sort(0);
	QTreeView::setModel(proxyModel);
}

QModelIndex EntityTreeView::mapToSource(const QModelIndex &proxyIndex) const
{
	return proxyModel->mapToSource(proxyIndex);
}
