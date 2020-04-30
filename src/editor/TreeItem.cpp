#include "TreeItem.hpp"
#include <QStringList>

TreeItem::TreeItem(const QString &title, TreeItem *parent)
{
	parentItem = parent;
	itemData << title;
}

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
	return childItems.value(row);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

int TreeItem::columnCount() const
{
	return itemData.count();
}

QVariant TreeItem::data(int column) const
{
	return itemData.value(column);
}

bool TreeItem::setData(int column, QVariant value)
{
	itemData.replace(column, value);
	return true;
}

TreeItem *TreeItem::parent()
{
	return parentItem;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QList<QVariant> data;
		data << "[empty]" << -1;
		TreeItem *item = new TreeItem(data, this);
		childItems.insert(position, item);
	}

	return true;
}

bool TreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

int TreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}
