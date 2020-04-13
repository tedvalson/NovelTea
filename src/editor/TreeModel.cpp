#include "TreeModel.hpp"
#include "TreeItem.hpp"
#include "Widgets/EditorTabWidget.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QStringList>
#include <QContextMenuEvent>
#include <QFont>
#include <QIcon>
#include <iostream>

TreeModel::TreeModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	rootItem = new TreeItem("Project");
}

TreeModel::~TreeModel()
{
	delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

void TreeModel::loadProject(const NovelTea::ProjectData &project)
{
	beginResetModel();

	delete rootItem;
	rootItem = new TreeItem("Project");

	roomRoot = new TreeItem("Rooms", rootItem);
	cutsceneRoot = new TreeItem("Cutscenes", rootItem);

	rootItem->appendChild(roomRoot);
	rootItem->appendChild(cutsceneRoot);

	cutsceneRoot->appendChild(new TreeItem("Apple", cutsceneRoot));
	cutsceneRoot->appendChild(new TreeItem("Dog", cutsceneRoot));
	cutsceneRoot->appendChild(new TreeItem("Banana", cutsceneRoot));
	cutsceneRoot->appendChild(new TreeItem("Zombie", cutsceneRoot));

	if (project.isLoaded())
	{
		auto j = project.data();
		for (auto &item : j[NT_CUTSCENES].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key()) << EditorTabWidget::Cutscene;
			cutsceneRoot->appendChild(new TreeItem(columnData, cutsceneRoot));

			std::cout << item.key() << std::endl;
		}
	}

	endResetModel();
}

void TreeModel::rename(EditorTabWidget::Type type, const QString &oldName, const QString &newName)
{
	TreeItem *parent = nullptr;
	if (type == EditorTabWidget::Cutscene)
		parent = cutsceneRoot;
	if (!parent)
		return;
	for (int i = 0; i < parent->childCount(); ++i)
	{
		auto child = parent->child(i);
		if (child->data(0) == oldName)
		{
			auto index = createIndex(i, 0, child);
			setData(index, newName);
			break;
		}
	}
}

void TreeModel::update()
{
	auto item = cutsceneRoot->child(0);
	auto i = createIndex(0, 0, item);
	if (!setData(i, "Kool test"))
		std::cout << "failed to update" << std::endl;

}

void TreeModel::contextMenuEvent(QContextMenuEvent *event)
{

}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	if (role == Qt::FontRole)
	{
		QFont font;
		if (item->parent() == rootItem)
			font.setBold(true);
		return font;
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.column() == 1)
			return QIcon::fromTheme("edit-undo");
	}
	else if (role == Qt::DisplayRole)
		return item->data(index.column());

	return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		bool success = item->setData(index.column(), value);
		if (success)
			emit dataChanged(index, index);
		return success;
	}

	return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	TreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}
