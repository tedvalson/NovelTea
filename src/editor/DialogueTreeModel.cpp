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

bool DialogueTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
	auto sourceParentItem = getItem(sourceParent);
	auto destParentItem = getItem(destinationParent);
	bool success = true;
	if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
		return false;
	for (int i = sourceRow + count-1; i >= sourceRow; --i)
	{
		auto item = sourceParentItem->child(i);
		item->moveItem(destParentItem, destinationChild);
	}
	endMoveRows();
	return success;
}

namespace {
void saveNodeLinks(const std::shared_ptr<NovelTea::Dialogue> &dialogue, DialogueTreeItem *node)
{
	for (int i = 0; i < node->childCount(); ++i)
		saveNodeLinks(dialogue, node->child(i));
	if (node->getLink())
		node->getDialogueSegment()->setLinkId(node->getLink()->getDialogueSegment()->getId());
}

int saveNode(const std::shared_ptr<NovelTea::Dialogue> &dialogue, DialogueTreeItem *node)
{
	auto segment = node->getDialogueSegment();
	segment->clearChildren();
	for (int i = 0; i < node->childCount(); ++i)
	{
		int childId = saveNode(dialogue, node->child(i));
		segment->appendChild(childId);
	}
	dialogue->addSegment(segment);
	return dialogue->segments().size() - 1;
}

void addNode(std::map<int, DialogueTreeItem*> &map, const std::shared_ptr<NovelTea::Dialogue> &dialogue, size_t segmentIndex, DialogueTreeItem *parentNode)
{
	auto &segment = dialogue->segments()[segmentIndex];
	auto node = new DialogueTreeItem(dialogue->getId(), segment, parentNode);
	map[segmentIndex] = node;
	parentNode->appendChild(node);
	for (auto childId : segment->getChildrenIds())
		addNode(map, dialogue, childId, node);
}
} // namespace

void DialogueTreeModel::saveDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue) const
{
	dialogue->clearSegments();
	auto rootIndex = saveNode(dialogue, m_rootItem->child(0));
	saveNodeLinks(dialogue, m_rootItem->child(0));
	dialogue->setRootIndex(rootIndex);
}

void DialogueTreeModel::loadDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue)
{
	beginResetModel();

	m_dialogueId = dialogue->getId();

	delete m_rootItem;
	m_rootItem = new DialogueTreeItem(m_dialogueId);

	// Add root node
	std::map<int, DialogueTreeItem*> map;
	addNode(map, dialogue, dialogue->getRootIndex(), m_rootItem);

	// Link nodes where needed
	for (auto &p : map)
	{
		auto &item = p.second;
		if (item->getDialogueSegment()->getType() == NovelTea::DialogueSegment::Link)
		{
			auto linkId = item->getDialogueSegment()->getLinkId();
			item->setLink(map[linkId]);
		}
	}

	endResetModel();
}

bool DialogueTreeModel::changeParent(const QModelIndex &child, const QModelIndex &newParent)
{
	auto childItem = getItem(child);
	auto parentItem = getItem(newParent);
	if (beginMoveRows(child.parent(), child.row(), child.row(), newParent, parentItem->childCount()))
	{
		childItem->moveItem(parentItem);
		endMoveRows();
		return true;
	}
	return false;
}

bool DialogueTreeModel::copy(const QModelIndex &source, const QModelIndex &destParent)
{
	auto parentItem = getItem(destParent);
	auto sourceItem = getItem(source);
	bool success;

	beginInsertRows(destParent, 0, 0);
	auto itemCopy = sourceItem->makeCopy(parentItem);
	success = parentItem->insertItem(0, itemCopy);
	endInsertRows();

	return success;
}

bool DialogueTreeModel::insertSegmentLink(const QModelIndex &source, const QModelIndex &destParent)
{
	auto parentItem = getItem(destParent);
	auto sourceItem = getItem(source);
	bool success;

	beginInsertRows(destParent, 0, 0);
	success = parentItem->insertLink(0, sourceItem);
	endInsertRows();

	return success;
}

bool DialogueTreeModel::insertSegment(int row, const QModelIndex &parent, const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	auto parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, row, row);
	success = parentItem->insertSegment(row, 1, segment);
	endInsertRows();

	return success;
}

bool DialogueTreeModel::updateSegment(const QModelIndex &index, const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	auto item = getItem(index);
	auto oldSegment = item->getDialogueSegment();
	if (oldSegment->getType() == NovelTea::DialogueSegment::Link)
	{
		item = item->getLink();
		oldSegment = item->getDialogueSegment();
	}
	if (*segment == *oldSegment)
		return false;
	item->setDialogueSegment(segment);
	// TODO: emit signal for all links too
	emit dataChanged(index, index);
	return true;
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
	auto segment = item->getDialogueSegment();
	auto type = segment->getType();

	if (role == Qt::FontRole)
	{
		QFont font;
		if (item->parent() == m_rootItem || segment->getTextRaw().empty())
			font.setItalic(true);
		return font;
	}
	else if (role == Qt::ForegroundRole)
	{
		QBrush brush;
		if (type == NovelTea::DialogueSegment::Option)
			brush.setColor(Qt::blue);
		else if (type == NovelTea::DialogueSegment::Text)
			brush.setColor(Qt::red);
		else if (type == NovelTea::DialogueSegment::Link)
			brush.setColor(Qt::gray);
		return brush;
	}
	else if (role == Qt::BackgroundRole)
	{
		if (segment->getScriptedText())
			return QBrush(QColor(Qt::yellow).lighter());
		else if(segment->getScriptEnabled())
			return QBrush(QColor(Qt::gray).lighter());
	}
	else if (role == Qt::DecorationRole)
	{
		if (segment->getScriptedText())
		{
			bool ok;
			auto text = segment->getText(&ok);
			if (!ok)
				return QIcon::fromTheme("dialog-error");
		}
		if (segment->getConditionalEnabled())
			return QIcon::fromTheme("emblem-important");
		if (segment->getType() != NovelTea::DialogueSegment::Root)
			return QColor(Qt::transparent);
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
