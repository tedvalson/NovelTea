#include "DialogueTreeItem.hpp"
#include <QStringList>

DialogueTreeItem::DialogueTreeItem(const std::string &title, DialogueTreeItem *parent)
{
	parentItem = parent;
	itemData << QString::fromStdString(title);
}

DialogueTreeItem::DialogueTreeItem(const std::shared_ptr<NovelTea::DialogueSegment> &segment, DialogueTreeItem *parent)
{
	parentItem = parent;
	loadDialogueSegment(segment);
}

DialogueTreeItem::~DialogueTreeItem()
{
	qDeleteAll(childItems);
}

void DialogueTreeItem::appendChild(DialogueTreeItem *item)
{
	childItems.append(item);
}

DialogueTreeItem *DialogueTreeItem::child(int row)
{
	return childItems.value(row);
}

int DialogueTreeItem::childCount() const
{
	return childItems.count();
}

int DialogueTreeItem::columnCount() const
{
	return itemData.count();
}

QVariant DialogueTreeItem::data(int column) const
{
	return itemData.value(column);
}

bool DialogueTreeItem::setData(int column, QVariant value)
{
	itemData.replace(column, value);
	return true;
}

DialogueTreeItem *DialogueTreeItem::parent()
{
	return parentItem;
}

void DialogueTreeItem::changeParent(DialogueTreeItem *parent)
{
	parentItem->childItems.takeAt(row());
	parent->appendChild(this);
	parentItem = parent;
}

void DialogueTreeItem::saveDialogueSegment(std::shared_ptr<NovelTea::DialogueSegment> segment) const
{
	auto type = static_cast<NovelTea::DialogueSegment::Type>(itemData[1].toInt());
	if (type != NovelTea::DialogueSegment::Root)
		segment->setText(itemData[0].toString().toStdString());
	segment->setType(type);
}

void DialogueTreeItem::loadDialogueSegment(const std::shared_ptr<NovelTea::DialogueSegment> segment)
{
	auto type = segment->getType();
	itemData.clear();
	if (type == NovelTea::DialogueSegment::Root)
		itemData << "ROOT";
	else
		itemData << QString::fromStdString(segment->getText());
	itemData << type;
}

bool DialogueTreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		auto segment = std::make_shared<NovelTea::DialogueSegment>();
		DialogueTreeItem *item = new DialogueTreeItem(segment, this);
		childItems.insert(position, item);
	}

	return true;
}

bool DialogueTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

int DialogueTreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<DialogueTreeItem*>(this));

	return 0;
}
