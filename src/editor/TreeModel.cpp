#include "TreeModel.hpp"
#include "TreeItem.hpp"
#include "EditorUtils.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
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
		return 1;
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

void addToJson(json *jout, const json &jitem, const json& jdata, std::vector<std::string> keys)
{
	auto id = jitem[0].ToString();
	auto parentId = jitem[1].ToString();
	keys.push_back(id);
	if (id == parentId) // Shouldn't be possible
		return;
	if (parentId.empty())
	{
		for (auto it = keys.rbegin(); it != keys.rend(); ++it)
			jout = &((*jout)[*it]);
		if (jout->IsEmpty())
			*jout = sj::Object();
	}
	else if (jdata.hasKey(parentId))
		addToJson(jout, jdata[parentId], jdata, keys);
}

void addToTree(const json &data, TreeItem *parent, NovelTea::EntityType type)
{
	for (auto &item : data.ObjectRange())
	{
		QList<QVariant> columnData;
		columnData << QString::fromStdString(item.first);
		columnData << static_cast<int>(type);
		columnData << QVariant();

		auto child = new TreeItem(columnData, parent);
		parent->appendChild(child);

		auto j = item.second;
		if (!j.IsEmpty())
			addToTree(j, child, type);
	}
}

void loadColors(TreeItem *item, const sj::JSON &jcolors)
{
	auto id = item->data(0).toString().toStdString();
	if (jcolors.hasKey(id))
		item->setData(2, QBrush(QColor(QString::fromStdString(jcolors[id].ToString()))));
	for (int i = 0; i < item->childCount(); ++i)
		loadColors(item->child(i), jcolors);
}

void loadEntities(const json &data, TreeItem *root, NovelTea::EntityType type, std::string typeIndex)
{
	if (!data.hasKey(typeIndex))
		return;

	std::vector<std::string> keys;
	auto j = sj::Object();
	for (auto &item : data[typeIndex].ObjectRange())
		addToJson(&j, item.second, data[typeIndex], keys);

	root->setData(1, static_cast<int>(type));
	addToTree(j, root, type);

	// Load item colors
	for (int i = 0; i < root->childCount(); ++i)
		loadColors(root->child(i), data[NovelTea::ID::entityColors][typeIndex]);
}

void TreeModel::loadProject(const NovelTea::ProjectData &project)
{
	beginResetModel();

	delete rootItem;
	rootItem = new TreeItem("Project");

	actionRoot   = new TreeItem("Actions", rootItem);
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
		auto jdata = project.data();

		loadEntities(jdata, actionRoot, NovelTea::EntityType::Action, NovelTea::Action::id);
		loadEntities(jdata, cutsceneRoot, NovelTea::EntityType::Cutscene, NovelTea::Cutscene::id);
		loadEntities(jdata, dialogueRoot, NovelTea::EntityType::Dialogue, NovelTea::Dialogue::id);
		loadEntities(jdata, objectRoot, NovelTea::EntityType::Object, NovelTea::Object::id);
		loadEntities(jdata, roomRoot, NovelTea::EntityType::Room, NovelTea::Room::id);
		loadEntities(jdata, scriptRoot, NovelTea::EntityType::Script, NovelTea::Script::id);
		loadEntities(jdata, verbRoot, NovelTea::EntityType::Verb, NovelTea::Verb::id);
	}

	endResetModel();
}


bool TreeModel::rename(TreeItem *item, int row, const QString &oldName, const QString &newName)
{
	if (item->data(0) == oldName)
	{
		auto index = createIndex(row, 0, item);
		setData(index, newName);
		return true;
	}

	for (int i = 0; i < item->childCount(); ++i)
		if (rename(item->child(i), i, oldName, newName))
			return true;
	return false;
}

void TreeModel::rename(EditorTabWidget::Type type, const QString &oldName, const QString &newName)
{
	auto parent = static_cast<TreeItem*>(index(type).internalPointer());
	if (!parent)
		return;
	for (int i = 0; i < parent->childCount(); ++i)
		if (rename(parent->child(i), i, oldName, newName))
			return;
}

void saveColorsRecursive(sj::JSON &jout, const TreeItem *item)
{
	for (int i = 0; i < item->childCount(); ++i)
		saveColorsRecursive(jout, item->child(i));

	auto id = item->data(0).toString().toStdString();
	auto color = item->data(2);
	if (color.isValid())
		jout[id] = color.value<QBrush>().color().name().toStdString();
}

void saveColors(sj::JSON &jout, const TreeItem *item)
{
	for (int i = 0; i < item->childCount(); ++i)
		saveColorsRecursive(jout, item->child(i));
}

void TreeModel::setColor(const QModelIndex &index, const QColor &color)
{
	auto item = getItem(index);
	if (!item)
		return;

	item->setData(2, color.isValid() ? QBrush(color) : QVariant());
}

sj::JSON TreeModel::getColorJSON() const
{
	auto result = sj::Object();
	saveColors(result[NovelTea::Action::id], actionRoot);
	saveColors(result[NovelTea::Cutscene::id], cutsceneRoot);
	saveColors(result[NovelTea::Dialogue::id], dialogueRoot);
	saveColors(result[NovelTea::Object::id], objectRoot);
	saveColors(result[NovelTea::Room::id], roomRoot);
	saveColors(result[NovelTea::Script::id], scriptRoot);
	saveColors(result[NovelTea::Verb::id], verbRoot);
	return result;
}

bool TreeModel::changeParent(const QModelIndex &child, const QModelIndex &newParent)
{
	auto childItem = getItem(child);
	auto parentItem = getItem(newParent);
	if (beginMoveRows(child.parent(), child.row(), child.row(), newParent, parentItem->childCount()))
	{
		std::cout << "change parent" << std::endl;
		childItem->changeParent(parentItem);
		endMoveRows();
		return true;
	}
	return false;
}

void TreeModel::update()
{

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
		if (!index.parent().isValid()) {
			auto tabType = EditorTabWidget::entityTypeToTabType(static_cast<NovelTea::EntityType>(item->data(1).toInt()));
			return EditorUtils::iconFromTabType(tabType);
		}
	}
	else if (role == Qt::DisplayRole)
		return item->data(index.column());
	else if (role == Qt::BackgroundRole)
		return item->data(2);

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
	int row;
	if (tabType == EditorTabWidget::Action)
		row = 0;
	else if (tabType == EditorTabWidget::Cutscene)
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
	else
		return QModelIndex();

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
