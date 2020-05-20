#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Widgets/EditorTabWidget.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>


class TreeItem;
class QContextMenuEvent;

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit TreeModel(QObject *parent = 0);
	~TreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());

	void loadProject(const NovelTea::ProjectData &project);
	void rename(NovelTea::EntityType type, const QString &oldName, const QString &newName);
	void update();

private:
	TreeItem *getItem(const QModelIndex &index) const;

	TreeItem *rootItem;
	TreeItem *cutsceneRoot;
	TreeItem *roomRoot;
};

#endif // TREEMODEL_H
