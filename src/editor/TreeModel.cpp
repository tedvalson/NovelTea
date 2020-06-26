#include "TreeModel.hpp"
#include "TreeItem.hpp"
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

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

void TreeModel::loadProject(const NovelTea::ProjectData &project)
{
	beginResetModel();

	delete rootItem;
	rootItem = new TreeItem("Project");

	actionRoot = new TreeItem("Actions", rootItem);
	cutsceneRoot = new TreeItem("Cutscenes", rootItem);
	dialogueRoot = new TreeItem("Dialogues", rootItem);
	objectRoot   = new TreeItem("Objects", rootItem);
	roomRoot     = new TreeItem("Rooms", rootItem);
	scriptRoot   = new TreeItem("Scripts", rootItem);
	verbRoot     = new TreeItem("Verbs", rootItem);

	rootItem->appendChild(actionRoot);
	rootItem->appendChild(cutsceneRoot);
	rootItem->appendChild(dialogueRoot);
	rootItem->appendChild(objectRoot);
	rootItem->appendChild(roomRoot);
	rootItem->appendChild(scriptRoot);
	rootItem->appendChild(verbRoot);

	if (project.isLoaded())
	{
		auto j = project.data();
		for (auto &item : j[NovelTea::ID::actions].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key());
			columnData << static_cast<int>(NovelTea::EntityType::Action);
			actionRoot->appendChild(new TreeItem(columnData, actionRoot));
		}
		for (auto &item : j[NovelTea::ID::cutscenes].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key());
			columnData << static_cast<int>(NovelTea::EntityType::Cutscene);
			cutsceneRoot->appendChild(new TreeItem(columnData, cutsceneRoot));
		}
		for (auto &item : j[NovelTea::ID::rooms].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key());
			columnData << static_cast<int>(NovelTea::EntityType::Room);
			roomRoot->appendChild(new TreeItem(columnData, roomRoot));
		}
		for (auto &item : j[NovelTea::ID::objects].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key());
			columnData << static_cast<int>(NovelTea::EntityType::Object);
			objectRoot->appendChild(new TreeItem(columnData, objectRoot));
		}
		for (auto &item : j[NovelTea::ID::verbs].items())
		{
			QList<QVariant> columnData;
			columnData << QString::fromStdString(item.key());
			columnData << static_cast<int>(NovelTea::EntityType::Verb);
			verbRoot->appendChild(new TreeItem(columnData, verbRoot));
		}
	}

	endResetModel();
}

void TreeModel::rename(NovelTea::EntityType type, const QString &oldName, const QString &newName)
{
	TreeItem *parent = nullptr;
	if (type == NovelTea::EntityType::Cutscene)
	{
		parent = cutsceneRoot;
		// TODO: rename all references to this cutscene in the project
		// NovelTea::Cutscene::rename (?) or more generic
	}

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

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
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

QModelIndex TreeModel::index(EditorTabWidget::Type tabType) const
{
	int row = 0;
	if (tabType == EditorTabWidget::Cutscene)
		row = 1;
	else if (tabType == EditorTabWidget::Dialogue)
		row = 2;
	else if (tabType == EditorTabWidget::Object)
		row = 3;
	else if (tabType == EditorTabWidget::Room)
		row = 4;
	else if (tabType == EditorTabWidget::Script)
		row = 5;
	else if (tabType == EditorTabWidget::Verb)
		row = 6;

	return index(row, 0);
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
