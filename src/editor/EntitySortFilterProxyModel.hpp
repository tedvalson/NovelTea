#ifndef ENTITYSORTFILTERPROXYMODEL_HPP
#define ENTITYSORTFILTERPROXYMODEL_HPP

#include <NovelTea/ProjectData.hpp>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QVariant>


class EntitySortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit EntitySortFilterProxyModel(QObject *parent = 0);
	~EntitySortFilterProxyModel();

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:
};

#endif // ENTITYSORTFILTERPROXYMODEL_HPP
