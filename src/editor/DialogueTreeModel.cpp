#include "DialogueTreeModel.hpp"
#include "DialogueTreeItem.hpp"
#include <QStringList>
#include <QFont>
#include <QIcon>
#include <iostream>

DialogueTreeModel::DialogueTreeModel(QObject *parent)
	: QAbstractItemModel(parent)
	, m_rootItem(new DialogueTreeItem(""))
{

}

DialogueTreeModel::~DialogueTreeModel()
{
	delete m_rootItem;
}

int DialogueTreeModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return getItem(parent)->columnCount();
	else
		return m_rootItem->columnCount();
}

bool DialogueTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	DialogueTreeItem *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, m_rootItem->columnCount());
	endInsertRows();

	return success;
}

bool DialogueTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	DialogueTreeItem *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

namespace {
int saveNode(const std::shared_ptr<NovelTea::Dialogue> &dialogue, DialogueTreeItem *node)
{
	auto segment = std::make_shared<NovelTea::DialogueSegment>();
	node->saveDialogueSegment(segment);
	for (int i = 0; i < node->childCount(); ++i)
	{
		int childId = saveNode(dialogue, node->child(i));
		segment->appendChild(childId);
	}
	dialogue->addSegment(segment);
	return dialogue->segments().size() - 1;
}

void addNode(const std::shared_ptr<NovelTea::Dialogue> &dialogue, size_t segmentIndex, DialogueTreeItem *parentNode)
{
	auto &segment = dialogue->segments()[segmentIndex];
	auto node = new DialogueTreeItem(segment, parentNode);
	parentNode->appendChild(node);
	for (auto childId : segment->getChildrenIds())
		addNode(dialogue, childId, node);
}
}

void DialogueTreeModel::saveDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue) const
{
	dialogue->clearSegments();
	auto rootIndex = saveNode(dialogue, m_rootItem->child(0));
	dialogue->setRootIndex(rootIndex);
}

void DialogueTreeModel::loadDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue)
{
	beginResetModel();

	delete m_rootItem;
	m_rootItem = new DialogueTreeItem("");

	// Add root node
	addNode(dialogue, dialogue->getRootIndex(), m_rootItem);

	endResetModel();
}

bool DialogueTreeModel::changeParent(const QModelIndex &child, const QModelIndex &newParent)
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

DialogueTreeItem *DialogueTreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		DialogueTreeItem *item = static_cast<DialogueTreeItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return m_rootItem;
}

QVariant DialogueTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto item = getItem(index);

	if (role == Qt::FontRole)
	{
		QFont font;
		if (item->parent() == m_rootItem)
			font.setItalic(true);
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

bool DialogueTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	auto item = getItem(index);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		bool success = item->setData(index.column(), value);
		if (success)
			emit dataChanged(index, index);
		return success;
	}

	return false;
}

Qt::ItemFlags DialogueTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return QAbstractItemModel::flags(index);
}

QModelIndex DialogueTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	DialogueTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = getItem(parent);

	DialogueTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex DialogueTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto childItem = getItem(index);
	auto parentItem = childItem->parent();

	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int DialogueTreeModel::rowCount(const QModelIndex &parent) const
{
	DialogueTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = getItem(parent);

	return parentItem->childCount();
}
