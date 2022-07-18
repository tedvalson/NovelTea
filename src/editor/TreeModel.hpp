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

	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex index(EditorTabWidget::Type tabType) const;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;

	bool insertEntity(const std::string &idName, EditorTabWidget::Type type, const QModelIndex &parent = QModelIndex());

	void loadProject(const NovelTea::ProjectData &project);
	void rename(EditorTabWidget::Type type, const QString &oldName, const QString &newName);
	bool changeParent(const QModelIndex &child, const QModelIndex &newParent);
	void update();

	void setColor(const QModelIndex &index, const QColor &color);
	sj::JSON getColorJSON() const;

private:
	bool rename(TreeItem *item, int row, const QString &oldName, const QString &newName);
	TreeItem *getItem(const QModelIndex &index) const;
	TreeItem *rootItem;

	TreeItem *actionRoot;
	TreeItem *cutsceneRoot;
	TreeItem *dialogueRoot;
	TreeItem *mapRoot;
	TreeItem *objectRoot;
	TreeItem *roomRoot;
	TreeItem *scriptRoot;
	TreeItem *verbRoot;
};

#endif // TREEMODEL_H
