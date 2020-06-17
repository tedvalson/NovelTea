#include "EntitySortFilterProxyModel.hpp"
#include <iostream>

EntitySortFilterProxyModel::EntitySortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{

}

EntitySortFilterProxyModel::~EntitySortFilterProxyModel()
{

}

bool EntitySortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
	if (!source_parent.isValid() && !sourceModel()->data(index).toString().contains(filterRegExp()))
		return false;
	return true;
}
